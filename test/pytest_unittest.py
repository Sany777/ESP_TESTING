# # SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# # SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut

@pytest.mark.esp32
@pytest.mark.supported_targets
@pytest.mark.generic




# def test_basic_expect(redirect, dut: Dut):
#     with redirect():
#         print('this would be redirected')

#     dut.expect('this')
#     dut.expect_exact('would')
#     dut.expect('[be]{2}')
#     dut.expect_exact('redirected')

# @pytest.mark.parametrize(
#     'port',
#     ['socket://localhost:9876'],
#     indirect=True,
# )
# def test_echo_tcp(dut):
#     dut.write(b'Hello, DUT!')
#     dut.expect('Hello, DUT!')  # will decode automatically

def test_unit_test(dut: Dut, test_name="") -> None:
    if test_name == "TEST_ALL":
        dut.run_all_single_board_cases()
    elif test_name[:11] == "TEST_GROUP=":
        dut.run_all_single_board_cases(test_name[12:])
    else:
        dut.run_single_board_case(test_name)

def test_restore_serial_settings(dut: Dut):

    dut.expect("test restore serial monitor settings", 10)
    dut.expect("serial monitor settings was restored", 15)