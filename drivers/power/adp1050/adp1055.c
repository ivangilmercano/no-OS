/***************************************************************************//**
 *   @file   adp1055.c
 *   @brief  Source file for the ADP1055 Driver
 *   @author Ivangil Mercano (Ivangil.mercano@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adp1055.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_crc8.h"

/******************************************************************************/
/************************ Variable Declarations ******************************/
/******************************************************************************/
NO_OS_DECLARE_CRC8_TABLE(adp1055_crc8);

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief Send command byte/word to ADP1055
 * @param desc - ADP1055 device descriptor
 * @param command - Value of the command
 * @param return 0 in case of sucess, negative error code otherwise
 */
int adp1055_send_command(struct adp1055_desc *desc uint16_t command)
{
    uint8_t val[ADP1055_SC_FRAME_SIZE] = {0, 0, 0};
    uint8_t ret;
    
    if (!desc)
        return -EINVAL;

    val[0] = (desc->i2c_desc->slave_address << 1);
    val[1] = command;

    if (desc->pece)
        val[2] = no_os_crc8(adp1055_crc8, val, 1, 0);
    
    ret = no_os_i2c_write(desc->i2c_desc, val, 1, 1);


}
/**
 * @brief - Write data to ADP1055
 * @param desc - ADP1055 device structure
 * @param command - Command value
 * @param data - Data value to write in ADP1055
 * @return 0 in case of success, negative error code otherwise
 */
int adp1055_write(struct adp1055_desc *desc, uint16_t command, uint16_t data)
{
    uint8_t val[ADP1055_WR_FRAME_SIZE] = {0, 0, 0, 0};
    uint8_t byte_num;
    
    if (!desc)
        return -EINVAL;

    byte_num = (desc->pece) ? (ADP1055_WR_FRAME_SIZE -1) : 2;
    val[0] = (desc->i2c_desc->slave_address << 1);
    val[1] = command;
    val[2] = no_os_field_get(ADP1055_LSB_MASK, data);

    if (desc->pece)
        val[3] = no_os_crc8(adp1055_crc8, val, byte_num, 0);
    
    return no_os_i2c_write(desc->i2c_desc, &val[1], byte_num, 1);
}

int adp1055_write(struct adp1055_desc *desc, uint16_t command, uint16_t data, uint8_t byte_num)
{
    uint8_t val[5] = {0, 0, 0, 0, 0};
    uint8_t crc_frame = 4;

    if (!desc)
        return -EINVAL;

    val[0] = (desc->i2c_desc->slave_address << 1);

    if (command > ADP1055_EXTENDED_COMMAND){
        val[1] = no_os_field_get(ADP1055_MSB_MASK, command);
        val[2] = no_os_field_get(ADP1055_LSB_MASK, command);
        
        if (byte_num > 1){
            val[3] = no_os_field_get(ADP1055_LSB_MASK, data);
            val[4] = no_os_field_get(ADP1055_MSB_MASK, data);
            crc_frame = 5;
        } else
            val[3] = no_os_field_get(ADP1055_LSB_MASK, data);

        val[crc_frame] = no_os_crc8(adp1055_crc8, val, crc_frame - 1 , 0);

        return no_os_i2c_write(desc->i2c_desc, &val[1], byte_num + 3, 1);

    } else {
        val[1] = no_os_field_get(ADP1055_LSB_MASK, command);
        if (byte_num > 1){
            val[2] = no_os_field_get(ADP1055_LSB_MASK, data);
            val[3] = no_os_field_get(ADP1050_MSB_MASK, data);
        } else 
            val[2] = no_os_field_get(ADP1050_LSB_MASK, data);

        val[crc_frame] = no_os_crc8(adp1055_crc8, val, crc_frame - 1, 0);

        return no_os_i2c_write(desc->i2c_desc, &val[1], byte_num + 2, 1)
    }
}

/**
 * @brief Read data from ADP1055
 * @param desc - ADP1055 device descriptor
 * @param command - Command value
 * @param data - Buffer with received data
 * @return 0 if success, negative error otherwise
 */
int adp1055_read(struct adp1055_desc *desc, uint16_t command, uint8_t *data)
{
    int ret;
    uint8_t val[ADP1055_RD_FRAME_SIZE] = {0, 0, 0, 0, 0};
    uint8_t byte_num;
    uint8_t crc;

    if (!desc)
        return -EINVAL;
    
    /** Compute pec over entire i2c frame from the first S condition */
    val[0] = (desc->i2c_desc->slave_address << 1) ;
    val[1] = command;
    val[2] = (desc->i2c_desc->slave_address << 1) | 0x1;

    /** i2c write target adress */
    byte_num = 1;

    ret = no_os_i2c_write(desc->i2c_desc, &i2c_data[1], byte_num, 0);
    if (ret)
        return ret;
    
    /** Change read byte count if PECE is enabled (1-byte data. 1-byte PEC) */
    byte_num = (desc->) ? 2 : byte_num;

    ret = no_os_i2c_read(desc->i2c_desc, &i2c_data[3], byte_num, 1);
    if (ret)
        return ret;
    
    if(desc->pece){
        /** Compute crc over entire i2c frame */
        crc = no_os_crc8(adp1055_crc8, val, (ADP1055_RD_FRAME_SIZE - 1), 0);

        if (val[4] != crc)
            return -EIO;
    }

    *data = val[3];

    return 0;
}

/**
 * @brief Initialize the ADP1055 device.
 * @param desc - ADP1055 device descriptor
 * @param init_param - Initialization parameter containing information about the
 * 		       ADP1055 device to be initialized.
 * @return 0 in case of succes, negative error code otherwise
*/
int adp1055_init(struct adp1055_desc **desc, struct adp1055_init_param *init_param)
{
    struct adp1055_desc *descriptor;
    int ret;

	descriptor = (struct adp1050_desc *)no_os_calloc(sizeof(*descriptor), 1);
	if (!descriptor)
		return -ENOMEM;

    ret = no_os_i2c_init(&descriptor->i2c_desc, init_param->i2c_param);
        if (ret)
            goto free_desc;

    ret = adp1055_write(descriptor, ADP1050_OPERATION, ADP1050_OPERATION_ON, 1);
        if (ret)
            goto free_desc;

    ret = adp1055_write(decriptor, ADP1050_ON_OFF_CONFIG,
                (uint16_t)init_param->on_off_config, 1);
        if (ret)
            goto free_desc;

free_desc:
	adp1050_remove(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by the adp1055_init()
 * @param desc - ADP1055 device descriptor
 * @return 0 in case of success, negative error otherwise
 */
int adp1055_remove(struct adp1055_desc *desc)
{
    int ret;

    if (!desc)
        return -ENODEV;

    if (desc->i2c_desc) {
        ret = adp1055_write(desc, ADP1055_OPERATION, ADP1055_OPERATION_OFF, 1);
        if (ret)
            return ret;
    }

    no_os_i2c_remove(desc->i2c_desc);
    no_os_free(desc);

    return 0;
}
