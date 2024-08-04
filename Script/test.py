import serial
import os,sys
import hdlc
os.chdir(os.path.dirname(__file__))
import simple_pb2
import pyqtgraph as pg
# from pyqtgraph.Qt import QtWidgets, QtCore
from PyQt5 import QtCore,QtWidgets
import numpy as np
import threading
import time

def print_byte(Bytes):
    print(['0x{:02X}'.format(byte) for byte in Bytes])

# port = '/dev/ttyACM0' 
port = 'COM5'
baud_rate = 9600 
ser = serial.Serial(port, baud_rate)
ser.reset_input_buffer()

def read_all_serial_data():
    if ser.in_waiting > 0:
        data = ser.read(ser.in_waiting)
        return data

def read_protobuf_data(data):
    # data = read_all_serial_data()
    message = simple_pb2.SimpleMessage()
    message.ParseFromString(data)
    return message

def write(data):
    encoded = hdlc.hdlc_encode(data)
    ser.write(encoded)

class WorkerThread(QtCore.QThread):
    def __init__(self, parent=None, ser_update=None):
        super().__init__(parent)
        self.ser_update = ser_update
        self.is_running = True

    def run(self):
        while self.is_running:
            self.ser_update()
            time.sleep(0.001)

    def pause(self):
        self.is_running = False

    def resume(self):
        self.is_running = True
        self.start()

class RealTimePlotApp(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.data_cnt = 4
        self.show_window = False
        self.update_window = True
        self.show_cursor = True
        self.save_dir = './output'
        if not os.path.exists(self.save_dir):
            os.makedirs(self.save_dir)
        self.data_selected = []
        
        self.plot_widget = pg.GraphicsLayoutWidget(show=True)
        self.plot_widget.setWindowTitle('Real-time Plot')
        self.plot = self.plot_widget.addPlot()
        self.plot.setLabel('bottom', 'Index')
        self.plot.setLabel('left', 'Value')
        self.plot.showGrid(x=True, y=True)
        self.window_size = 50
        self.data_points_all = np.zeros((1, self.data_cnt))
        color_list = ['g', 'b', 'c', 'm', 'y']
        self.curve = [self.plot.plot(pen=color_list[i]) for i in range(self.data_cnt)]
        self.curve2 = [self.plot.plot(pen='r') for i in range(self.data_cnt)]

        self.legend_list = ['LED_R','LED_B','LED_R_B','state']
        self.legend = pg.LegendItem(offset=(70,30))
        self.legend.setParentItem(self.plot)
        for i in range(self.data_cnt):
            self.legend.addItem(self.curve[i], self.legend_list[i])

        self.worker_thread = WorkerThread(ser_update=self.ser_update)
        self.worker_thread.moveToThread(self.worker_thread)
        self.worker_thread.started.connect(self.worker_thread.run)
        self.worker_thread.start()

        self.label = pg.TextItem(anchor=(0, 1))
        self.plot.addItem(self.label)
        self.label.setHtml('<div style="text-align: center"><span style="color: #FFF;font-size: 12pt"></span></div>')  # Initial text
        self.plot_widget.scene().sigMouseMoved.connect(self.mouseMoveEvent)  # Connect mouse move event

        self.plot.scene().sigMouseClicked.connect(self.mousePressEvent)

        self.button = QtWidgets.QPushButton('Stop')
        self.button.clicked.connect(self.stop_button_clicked)
        self.pause_button = QtWidgets.QPushButton('Pause')
        self.pause_button.clicked.connect(self.pause_button_clicked)
        self.save_button = QtWidgets.QPushButton('save all')
        self.save_button.clicked.connect(self.save_button_clicked)
        self.save_selected_button = QtWidgets.QPushButton('save selected')
        self.save_selected_button.clicked.connect(self.save_selected_button_clicked)

        self.checkbox = QtWidgets.QCheckBox('show cursor', self)
        self.checkbox.setChecked(True)  
        self.checkbox.stateChanged.connect(self.checkbox_state_changed)

        layout = QtWidgets.QVBoxLayout(self)
        layout.addWidget(self.pause_button)
        layout.addWidget(self.button)
        layout.addWidget(self.save_button)
        layout.addWidget(self.save_selected_button)
        layout.addWidget(self.checkbox)

        self.central_widget = QtWidgets.QWidget()
        self.central_widget.setLayout(layout)
        self.setCentralWidget(self.central_widget)

    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.LeftButton:
            x, y = event.pos().x(), event.pos().y()
            plot_point = self.plot.vb.mapToView(QtCore.QPointF(x, y))
            plot_x = plot_point.x()
            plot_y = plot_point.y()
            data_x = range(len(self.data_points_all))
            data_y = self.data_points_all

            closest_index = np.argmin(np.abs(np.array(data_x) - plot_x))
            closest_data_x = data_x[closest_index]
            closest_data_y = data_y[closest_index]
            plot_x = closest_data_x
            plot_y = closest_data_y

            if (plot_x,*plot_y) in self.data_selected:
                self.data_selected.remove((plot_x, *plot_y))
            else:
                self.data_selected.append((plot_x, *plot_y))
            # import ipdb;ipdb.set_trace()
            data_array = np.array(self.data_selected).reshape(-1, self.data_cnt+1)
            for i,cur in enumerate(self.curve2):
                    cur.setData(data_array[:,0], data_array[:,i+1],symbol='o',pen=None)

            # self.curve2.setData(data_array[:, 0], data_array[:, 1],symbol='o',pen=None)

    def checkbox_state_changed(self, state):
        if state == 2: 
            print('Checkbox is checked')
            self.show_cursor = True
        else:
            print('Checkbox is unchecked')
            self.show_cursor = False

    def mouseMoveEvent(self, event):
        pos = event
        if self.show_cursor and self.plot.sceneBoundingRect().contains(pos):
            mouse_point = self.plot.vb.mapSceneToView(pos)
            x = mouse_point.x()
            y = mouse_point.y()
            self.label.setHtml(f'<div style="text-align: center"><span style="color: #FFF;font-size: 12pt">x={round(x,3)}, y={round(y,3)}</span></div>')
            self.label.setPos(mouse_point.x(), mouse_point.y()) 
        else:
            self.label.setHtml('<div style="text-align: center"><span style="color: #FFF;font-size: 12pt"></span></div>')

    def stop_button_clicked(self):
        print('Button clicked!')
        if self.worker_thread.is_running:
            self.worker_thread.pause()
        else:
            self.worker_thread.resume()
    
    def pause_button_clicked(self):
        if self.update_window:
            self.update_window = False
            self.pause_button.setText('Resume')
        else:
            self.update_window = True
            self.pause_button.setText('Pause')

    def save_button_clicked(self):
        with open(os.path.join(self.save_dir,'output.txt'), 'w') as file:
            file.writelines([str(i) + ',' + ','.join([str(it) for it in item]) + '\n' for i, item in enumerate(self.data_points_all)])
            print('save ok')

    def save_selected_button_clicked(self):
        with open(os.path.join(self.save_dir,'output_selected.txt'), 'w') as file:
            data_selected = sorted(self.data_selected, key=lambda x: x[0])
            file.writelines([','.join([str(it) for it in item]) + '\n' for i, item in enumerate(data_selected)])
            print('save selected ok')

    def ser_update(self):
        # Get user input
        print('start read ... ')
        hdlc_buf = []
        byte = ser.read(1)[0]
        if byte == hdlc.HDLC_FLAG:
            hdlc_buf.append(byte)
            while True:
                byte = ser.read(1)[0]
                hdlc_buf.append(byte)
                if byte == hdlc.HDLC_FLAG: break
            hex_data = [hex(byte) for byte in hdlc_buf]
            print(hex_data)
            decoded = hdlc.hdlc_decode(hdlc_buf)
            print_byte(decoded)
            if decoded is None:
                print('CRC error')
                read_all_serial_data()
                return
            serialized = bytes(decoded)
            message = simple_pb2.SimpleMessage()
            message.ParseFromString(serialized)
            # import ipdb;ipdb.set_trace()
            print(message)
            data = np.array([message.LED_R, message.LED_B,message.LED_R_B,message.state]).reshape(1, self.data_cnt)
            self.img_update(data)
        else :
            data = read_all_serial_data()
            if data is None : return
            msg_str = chr(byte) + data.decode('utf-8', errors='ignore')
            print(f'Received: {msg_str}')

    def img_update(self, data):
        new_data = np.reshape(data, (1, -1))
        # import ipdb;ipdb.set_trace()
        self.data_points_all = np.vstack((self.data_points_all, new_data))
        if self.update_window:
            if self.show_window:
                data_points_display = self.data_points_all[-self.window_size:, :]
                self.curve.setData(range(len(data_points_display)), data_points_display)
            else :
                for i,cur in enumerate(self.curve):
                    cur.setData(range(len(self.data_points_all)), self.data_points_all[:,i])

if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication([])
    real_time_plot_app = RealTimePlotApp()
    real_time_plot_app.show()

    sys.exit(app.exec_())
    