# # SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# # SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic



def test_unit_test(dut: Dut, test_name: str) -> None:
    if test_name == "TEST_ALL":
        dut.run_all_single_board_cases()
    elif test_name[:11] == "TEST_GROUP=":
        dut.run_all_single_board_cases(test_name[12:])
    elif test_name not None:
        dut.run_single_board_case(test_name)