import pytest
from pytest_embedded import Dut

from time import sleep


@pytest.mark.esp32
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.unit
def test_basic_test(dut: Dut) -> None:
    
    dut.expect_unity_test_output(timeout = 2000)

    dut.write('[pytest]')

    dut.expect('serial monitor settings was restored')
    
    dut.expect('esp:send')
    dut.expect('esp:data')

    test_data = b'pytest:data'
    dut.expect('esp:expect data')
    dut.write(test_data)
    dut.expect('esp:ok')

