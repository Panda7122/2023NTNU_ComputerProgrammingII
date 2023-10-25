all:
	gcc hw0301.c -o hw0301
	gcc hw0302.c -o hw0302
	gcc hw0303.c -o hw0303
	gcc hw0304.c -o hw0304
	gcc hw0305.c -o hw0305
test0301:all
	./getcase
	./hw0301 -e -k key -i input -o cipher
	./hw0301 -d -k key -i cipher -o output
test0302:all
	./hw0302 test.zip > testdef
	./hw0302 -a test.zip > test-a
	./hw0302 -d test.zip > test-d
test0303:all
	./hw0303 -x 0 -y 200 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy1.bmp
	./hw0303 -x 100 -y 200 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy2.bmp
	./hw0303 -x 350 -y 200 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy3.bmp
	./hw0303 -x 100 -y 0 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy11.bmp
	./hw0303 -x 100 -y 100 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy12.bmp
	./hw0303 -x 100 -y 450 -w 1000 -h 1000 -n 10 -i spy.bmp -o spy13.bmp
	./hw0303 -x 0 -y 200 -w 10 -h 100 -n 10 -i spy.bmp -o spy01.bmp
	./hw0303 -x 0 -y 200 -w 20 -h 100 -n 10 -i spy.bmp -o spy02.bmp
