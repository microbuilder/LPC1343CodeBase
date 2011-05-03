/**************************************************************************/
/*! 
    @file     icons16.h
    @author   K. Townsend (microBuilder.eu)

    @section LICENSE

    Software License Agreement (BSD License)

    Copyright (c) 2010, microBuilder SARL
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
    1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holders nor the
    names of its contributors may be used to endorse or promote products
    derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**************************************************************************/

/*
    All icons have been converted from the GentleFace Toolbar Icon Set,
    licensed under the Creative Commons Attribution-NonCommercial License.
    for more information see: http://www.gentleface.com/free_icon_set.html
*/

// Each icon used will consume 32 bytes of flash memory

#ifndef __ICONS16_H__
#define __ICONS16_H__

uint16_t icons16_alert[16]            = { 0x0000, 0x0000, 0x0180, 0x03C0, 0x03C0, 0x0660, 0x0660, 0x0E70, 0x0E70, 0x1E78, 0x3E7C, 0x3FFC, 0x7E7E, 0x7E7E, 0xFFFF, 0x0000 };
uint16_t icons16_alert_interior[16]   = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0180, 0x0000, 0x0180, 0x0180, 0x0000, 0x0000 };
uint16_t icons16_info[16]             = { 0x0000, 0x07F0, 0x0FF8, 0x1FFC, 0x3F3E, 0x7F3F, 0x7FFF, 0x7F3F, 0x7F3F, 0x7F3F, 0x7F3F, 0x7F3F, 0x3F3E, 0x1FFC, 0x0FF8, 0x07F0 };
uint16_t icons16_info_interior[16]    = { 0x0000, 0x0000, 0x0000, 0x0000, 0x00C0, 0x00C0, 0x0000, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x00C0, 0x0000, 0x0000, 0x0000 };
uint16_t icons16_failed[16]           = { 0x0000, 0x07F0, 0x0FF8, 0x1FFC, 0x3FFE, 0x79CF, 0x788F, 0x7C1F, 0x7E3F, 0x7C1F, 0x788F, 0x79CF, 0x3FFE, 0x1FFC, 0x0FF8, 0x07F0 };
uint16_t icons16_failed_interior[16]  = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0630, 0x0770, 0x03E0, 0x01C0, 0x03E0, 0x0770, 0x0630, 0x0000, 0x0000, 0x0000, 0x0000 };
uint16_t icons16_passed[16]           = { 0x0000, 0x07F0, 0x0FF8, 0x1FFC, 0x3FFE, 0x7FEF, 0x7FC7, 0x7F8F, 0x731F, 0x783F, 0x7C7F, 0x7EFF, 0x3FFE, 0x1FFC, 0x0FF8, 0x07F0 };
uint16_t icons16_passed_interior[16]  = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0010, 0x0038, 0x0070, 0x0CE0, 0x07C0, 0x0380, 0x0100, 0x0000, 0x0000, 0x0000, 0x0000 };
uint16_t icons16_pointer[16]          = { 0x07C0, 0x1FF0, 0x3FF8, 0x3FF8, 0x7FFC, 0x7FFC, 0x7FFC, 0x7FFC, 0x7FFC, 0x7FFC, 0x3FF8, 0x1FF0, 0x0FE0, 0x07C0, 0x0380, 0x0100 };
uint16_t icons16_pointer_dot[16]      = { 0x0000, 0x0000, 0x0000, 0x0000, 0x0380, 0x07C0, 0x07C0, 0x07C0, 0x0380, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
uint16_t icons16_tag[16]              = { 0xFF00, 0xFF80, 0xCFC0, 0xCFE0, 0xFFF0, 0xFFF8, 0xFFFC, 0xFFFE, 0x7FFE, 0x3FFE, 0x1FFC, 0x0FF8, 0x07F0, 0x03E0, 0x01C0, 0x0000 };
uint16_t icons16_tag_dot[16]          = { 0x0000, 0x0000, 0x3000, 0x3000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000 };
uint16_t icons16_tools[16]            = { 0x118C, 0x118C, 0x11FC, 0x11FC, 0x10F8, 0x1070, 0x1070, 0x3870, 0x1070, 0x3870, 0x3870, 0x3870, 0x3870, 0x3870, 0x3870, 0x3870 };

#endif
