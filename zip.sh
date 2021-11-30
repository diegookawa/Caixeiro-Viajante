sudo apt install -y zip
ra="./2127890_2127997_1904965"
rm -rf "$ra.zip" $ra
mkdir $ra
cp Makefile tsp.c "$ra/"
zip "$ra.zip" -r $ra
rm -rf $ra