output_dir=../.. 
protoc --nanopb_out=. simple.proto 
mv ./*.c $output_dir/Src/
mv ./*.h $output_dir/Inc/
echo "c proto generated"

protoc --python_out=. simple.proto 
mv ./*.py $output_dir/Script/ 
echo "py proto generated"