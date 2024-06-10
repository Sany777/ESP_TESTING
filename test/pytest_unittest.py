# # SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# # SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


# import serial
# SERIAL_PORT = 'COM6'  # або '/dev/ttyUSB0' для Linux
# BAUD_RATE = 115200
# TIMEOUT = 1
# @pytest.fixture(scope='module')
# def uart():
#     try:
#         ser = serial.Serial(SERIAL_PORT, baudrate=BAUD_RATE, timeout=TIMEOUT)
#         time.sleep(2)  # Очікування для встановлення з'єднання
#         yield ser
#         ser.close()
#     except serial.SerialException as e:
#         pytest.fail(f"Could not open serial port {SERIAL_PORT}: {e}")


@pytest.mark.esp32
@pytest.mark.supported_targets
@pytest.mark.generic
def test_basic_expect(redirect, dut: Dut):
    test_data = b'pytest:data'
    dut.expect('expect data')
    dut.write(test_data)
    dut.expect_exact('esp ok')


# @pytest.mark.parametrize(
#     'port',
#     ['socket://localhost:9876'],
#     indirect=True,
# )
# def test_echo_tcp(dut):
#     dut.write(b'Hello, DUT!')
#     dut.expect('Hello, DUT!')  # will decode automatically

# def test_unit_test(dut: Dut, test_name="") -> None:
#     if test_name == "TEST_ALL":
#         dut.run_all_single_board_cases()
#     elif test_name[:11] == "TEST_GROUP=":
#         dut.run_all_single_board_cases(test_name[12:])
#     else:
#         dut.run_single_board_case(test_name)

def test_restore_serial_settings(dut: Dut):

    dut.expect("test restore serial monitor settings")
    dut.expect("serial monitor settings was restored")