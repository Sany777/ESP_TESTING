import pytest
from pytest_embedded import Dut

from time import sleep


@pytest.mark.esp32
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.unit
def test_basic_test(dut: Dut) -> None:
    
    dut.expect_unity_test_output(timeout = 2000)

    sleep(1)
    dut.write('1')
    dut.expect('serial monitor settings was restored')

    sleep(1)
    dut.write('2')
    dut.expect('esp:send')
    dut.expect('esp:data')

    sleep(1)
    dut.write('3')
    test_data = b'pytest:data'
    dut.expect('esp:expect data')
    dut.write(test_data)
    dut.expect('esp:ok')

