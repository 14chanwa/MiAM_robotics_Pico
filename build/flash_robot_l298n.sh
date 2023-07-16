make -j4 && openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program robot_l298n.elf verify reset exit"
