::     \file    image_prebuild.bat
::     \brief   Bat file for image prebuild.
::
::     \version 2023-07-20, V1.0.0, firmware for GD32VW55x
::
::     Copyright (c) 2023, GigaDevice Semiconductor Inc.
::
::     Redistribution and use in source and binary forms, with or without modification,
:: are permitted provided that the following conditions are met:
::
::     1. Redistributions of source code must retain the above copyright notice, this
::        list of conditions and the following disclaimer.
::     2. Redistributions in binary form must reproduce the above copyright notice,
::        this list of conditions and the following disclaimer in the documentation
::        and/or other materials provided with the distribution.
::     3. Neither the name of the copyright holder nor the names of its contributors
::        may be used to endorse or promote products derived from this software without
::        specific prior written permission.
::
::     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
:: AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
:: WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
:: IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
:: INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
:: NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
:: PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
:: WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
:: ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
:: OF SUCH DAMAGE.

@echo off
chcp 65001

set TOOLKIT=%1
set MSDK_DIR=%2
set MATTER=%4

::set MSDK_DIR=..\\..\\..\\..
set SCONS_CONFIG=%MSDK_DIR%\macsw\export\wlan_config.h
set PLATFORM_CONFIG=%MSDK_DIR%\..\config\platform_def.h

echo cd=%cd%

:: generate gd32vw55x.lds
if "%TOOLKIT%" neq "IAR" (
    if "%MATTER%" == "matter" (
        %TOOLKIT%gcc -E -DCFG_MATTER -P -o  %MSDK_DIR%\plf\riscv\env\gd32vw55x.lds ^
                        -x c-header %MSDK_DIR%\plf\riscv\env\gd32vw55x.ld ^
                        -I %MSDK_DIR%\..\config -I %MSDK_DIR%\..\MBL\mainboot ^
                        -I %MSDK_DIR%\macsw\export -I %MSDK_DIR%\util\include
        ) else (
        %TOOLKIT%gcc -E -P -o %MSDK_DIR%\plf\riscv\env\gd32vw55x.lds ^
                        -x c-header %MSDK_DIR%\plf\riscv\env\gd32vw55x.ld ^
                        -I %MSDK_DIR%\..\config -I %MSDK_DIR%\..\MBL\mainboot ^
                        -I %MSDK_DIR%\macsw\export -I %MSDK_DIR%\util\include
        )
)

chcp 437
echo /* Do not change the content here, it's auto generated */ > %MSDK_DIR%\app\_build_date.h

REM build date
echo #define SDK_BUILD_DATE "%DATE:~-10% %TIME:~0,-3%" >> %MSDK_DIR%\app\_build_date.h

goto end

:: generate trace file ids
python ..\\..\\gen_trace_id.py ..\\..\\msdk\\msdk_out

:end
