		
composite Main()
{
	stream<int a,int b>S;
	S = source()
	{
		
		//   色度AC huffman 表
		int s_lumin_ac[256][2] = { 
  
         { 4, 0x000a}, /* 0x00:              1010 */ 
         { 2, 0x0000}, /* 0x01:                00 */ 
         { 2, 0x0001}, /* 0x02:                01 */ 
         { 3, 0x0004}, /* 0x03:               100 */ 
         { 4, 0x000b}, /* 0x04:              1011 */ 
         { 5, 0x001a}, /* 0x05:             11010 */ 
         { 7, 0x0078}, /* 0x06:           1111000 */ 
         { 8, 0x00f8}, /* 0x07:          11111000 */ 
         {10, 0x03f6}, /* 0x08:        1111110110 */ 
         {16, 0xff82}, /* 0x09:  1111111110000010 */ 
         {16, 0xff83}, /* 0x0a:  1111111110000011 */ 
         { 0, 0x0000}, /* 0x0b:                   */ 
         { 0, 0x0000}, /* 0x0c:                   */ 
         { 0, 0x0000}, /* 0x0d:                   */ 
         { 0, 0x0000}, /* 0x0e:                   */ 
         { 0, 0x0000}, /* 0x0f:                   */ 
         { 0, 0x0000}, /* 0x10:                   */ 
         { 4, 0x000c}, /* 0x11:              1100 */ 
         { 5, 0x001b}, /* 0x12:             11011 */ 
         { 7, 0x0079}, /* 0x13:           1111001 */ 
         { 9, 0x01f6}, /* 0x14:         111110110 */ 
         {11, 0x07f6}, /* 0x15:       11111110110 */ 
         {16, 0xff84}, /* 0x16:  1111111110000100 */ 
         {16, 0xff85}, /* 0x17:  1111111110000101 */ 
         {16, 0xff86}, /* 0x18:  1111111110000110 */ 
         {16, 0xff87}, /* 0x19:  1111111110000111 */ 
         {16, 0xff88}, /* 0x1a:  1111111110001000 */ 
         { 0, 0x0000}, /* 0x1b:                   */ 
         { 0, 0x0000}, /* 0x1c:                   */ 
         { 0, 0x0000}, /* 0x1d:                   */ 
         { 0, 0x0000}, /* 0x1e:                   */ 
         { 0, 0x0000}, /* 0x1f:                   */ 
         { 0, 0x0000}, /* 0x20:                   */ 
         { 5, 0x001c}, /* 0x21:             11100 */ 
         { 8, 0x00f9}, /* 0x22:          11111001 */ 
         {10, 0x03f7}, /* 0x23:        1111110111 */ 
         {12, 0x0ff4}, /* 0x24:      111111110100 */ 
         {16, 0xff89}, /* 0x25:  1111111110001001 */ 
         {16, 0xff8a}, /* 0x26:  1111111110001010 */ 
         {16, 0xff8b}, /* 0x27:  1111111110001011 */ 
         {16, 0xff8c}, /* 0x28:  1111111110001100 */ 
         {16, 0xff8d}, /* 0x29:  1111111110001101 */ 
         {16, 0xff8e}, /* 0x2a:  1111111110001110 */ 
         { 0, 0x0000}, /* 0x2b:                   */ 
         { 0, 0x0000}, /* 0x2c:                   */ 
         { 0, 0x0000}, /* 0x2d:                   */ 
         { 0, 0x0000}, /* 0x2e:                   */ 
         { 0, 0x0000}, /* 0x2f:                   */ 
         { 0, 0x0000}, /* 0x30:                   */ 
         { 6, 0x003a}, /* 0x31:            111010 */ 
         { 9, 0x01f7}, /* 0x32:         111110111 */ 
         {12, 0x0ff5}, /* 0x33:      111111110101 */ 
         {16, 0xff8f}, /* 0x34:  1111111110001111 */ 
         {16, 0xff90}, /* 0x35:  1111111110010000 */ 
         {16, 0xff91}, /* 0x36:  1111111110010001 */ 
         {16, 0xff92}, /* 0x37:  1111111110010010 */ 
         {16, 0xff93}, /* 0x38:  1111111110010011 */ 
         {16, 0xff94}, /* 0x39:  1111111110010100 */ 
         {16, 0xff95}, /* 0x3a:  1111111110010101 */ 
         { 0, 0x0000}, /* 0x3b:                   */ 
         { 0, 0x0000}, /* 0x3c:                   */ 
         { 0, 0x0000}, /* 0x3d:                   */ 
         { 0, 0x0000}, /* 0x3e:                   */ 
         { 0, 0x0000}, /* 0x3f:                   */ 
         { 0, 0x0000}, /* 0x40:                   */ 
         { 6, 0x003b}, /* 0x41:            111011 */ 
         {10, 0x03f8}, /* 0x42:        1111111000 */ 
         {16, 0xff96}, /* 0x43:  1111111110010110 */ 
         {16, 0xff97}, /* 0x44:  1111111110010111 */ 
         {16, 0xff98}, /* 0x45:  1111111110011000 */ 
         {16, 0xff99}, /* 0x46:  1111111110011001 */ 
         {16, 0xff9a}, /* 0x47:  1111111110011010 */ 
         {16, 0xff9b}, /* 0x48:  1111111110011011 */ 
         {16, 0xff9c}, /* 0x49:  1111111110011100 */ 
         {16, 0xff9d}, /* 0x4a:  1111111110011101 */ 
         { 0, 0x0000}, /* 0x4b:                   */ 
         { 0, 0x0000}, /* 0x4c:                   */ 
         { 0, 0x0000}, /* 0x4d:                   */ 
         { 0, 0x0000}, /* 0x4e:                   */ 
         { 0, 0x0000}, /* 0x4f:                   */ 
         { 0, 0x0000}, /* 0x50:                   */ 
         { 7, 0x007a}, /* 0x51:           1111010 */ 
         {11, 0x07f7}, /* 0x52:       11111110111 */ 
         {16, 0xff9e}, /* 0x53:  1111111110011110 */ 
         {16, 0xff9f}, /* 0x54:  1111111110011111 */ 
         {16, 0xffa0}, /* 0x55:  1111111110100000 */ 
         {16, 0xffa1}, /* 0x56:  1111111110100001 */ 
         {16, 0xffa2}, /* 0x57:  1111111110100010 */ 
         {16, 0xffa3}, /* 0x58:  1111111110100011 */ 
         {16, 0xffa4}, /* 0x59:  1111111110100100 */ 
         {16, 0xffa5}, /* 0x5a:  1111111110100101 */ 
         { 0, 0x0000}, /* 0x5b:                   */ 
         { 0, 0x0000}, /* 0x5c:                   */ 
         { 0, 0x0000}, /* 0x5d:                   */ 
         { 0, 0x0000}, /* 0x5e:                   */ 
         { 0, 0x0000}, /* 0x5f:                   */ 
         { 0, 0x0000}, /* 0x60:                   */ 
         { 7, 0x007b}, /* 0x61:           1111011 */ 
         {12, 0x0ff6}, /* 0x62:      111111110110 */ 
         {16, 0xffa6}, /* 0x63:  1111111110100110 */ 
         {16, 0xffa7}, /* 0x64:  1111111110100111 */ 
         {16, 0xffa8}, /* 0x65:  1111111110101000 */ 
         {16, 0xffa9}, /* 0x66:  1111111110101001 */ 
         {16, 0xffaa}, /* 0x67:  1111111110101010 */ 
         {16, 0xffab}, /* 0x68:  1111111110101011 */ 
         {16, 0xffac}, /* 0x69:  1111111110101100 */ 
         {16, 0xffad}, /* 0x6a:  1111111110101101 */ 
         { 0, 0x0000}, /* 0x6b:                   */ 
         { 0, 0x0000}, /* 0x6c:                   */ 
         { 0, 0x0000}, /* 0x6d:                   */ 
         { 0, 0x0000}, /* 0x6e:                   */ 
         { 0, 0x0000}, /* 0x6f:                   */ 
         { 0, 0x0000}, /* 0x70:                   */ 
         { 8, 0x00fa}, /* 0x71:          11111010 */ 
         {12, 0x0ff7}, /* 0x72:      111111110111 */ 
         {16, 0xffae}, /* 0x73:  1111111110101110 */ 
         {16, 0xffaf}, /* 0x74:  1111111110101111 */ 
         {16, 0xffb0}, /* 0x75:  1111111110110000 */ 
         {16, 0xffb1}, /* 0x76:  1111111110110001 */ 
         {16, 0xffb2}, /* 0x77:  1111111110110010 */ 
         {16, 0xffb3}, /* 0x78:  1111111110110011 */ 
         {16, 0xffb4}, /* 0x79:  1111111110110100 */ 
         {16, 0xffb5}, /* 0x7a:  1111111110110101 */ 
         { 0, 0x0000}, /* 0x7b:                   */ 
         { 0, 0x0000}, /* 0x7c:                   */ 
         { 0, 0x0000}, /* 0x7d:                   */ 
         { 0, 0x0000}, /* 0x7e:                   */ 
         { 0, 0x0000}, /* 0x7f:                   */ 
         { 0, 0x0000}, /* 0x80:                   */ 
         { 9, 0x01f8}, /* 0x81:         111111000 */ 
         {15, 0x7fc0}, /* 0x82:   111111111000000 */ 
         {16, 0xffb6}, /* 0x83:  1111111110110110 */ 
         {16, 0xffb7}, /* 0x84:  1111111110110111 */ 
         {16, 0xffb8}, /* 0x85:  1111111110111000 */ 
         {16, 0xffb9}, /* 0x86:  1111111110111001 */ 
         {16, 0xffba}, /* 0x87:  1111111110111010 */ 
         {16, 0xffbb}, /* 0x88:  1111111110111011 */ 
         {16, 0xffbc}, /* 0x89:  1111111110111100 */ 
         {16, 0xffbd}, /* 0x8a:  1111111110111101 */ 
         { 0, 0x0000}, /* 0x8b:                   */ 
         { 0, 0x0000}, /* 0x8c:                   */ 
         { 0, 0x0000}, /* 0x8d:                   */ 
         { 0, 0x0000}, /* 0x8e:                   */ 
         { 0, 0x0000}, /* 0x8f:                   */ 
         { 0, 0x0000}, /* 0x90:                   */ 
         { 9, 0x01f9}, /* 0x91:         111111001 */ 
         {16, 0xffbe}, /* 0x92:  1111111110111110 */ 
         {16, 0xffbf}, /* 0x93:  1111111110111111 */ 
         {16, 0xffc0}, /* 0x94:  1111111111000000 */ 
         {16, 0xffc1}, /* 0x95:  1111111111000001 */ 
         {16, 0xffc2}, /* 0x96:  1111111111000010 */ 
         {16, 0xffc3}, /* 0x97:  1111111111000011 */ 
         {16, 0xffc4}, /* 0x98:  1111111111000100 */ 
         {16, 0xffc5}, /* 0x99:  1111111111000101 */ 
         {16, 0xffc6}, /* 0x9a:  1111111111000110 */ 
         { 0, 0x0000}, /* 0x9b:                   */ 
         { 0, 0x0000}, /* 0x9c:                   */ 
         { 0, 0x0000}, /* 0x9d:                   */ 
         { 0, 0x0000}, /* 0x9e:                   */ 
         { 0, 0x0000}, /* 0x9f:                   */ 
         { 0, 0x0000}, /* 0xa0:                   */ 
         { 9, 0x01fa}, /* 0xa1:         111111010 */ 
         {16, 0xffc7}, /* 0xa2:  1111111111000111 */ 
         {16, 0xffc8}, /* 0xa3:  1111111111001000 */ 
         {16, 0xffc9}, /* 0xa4:  1111111111001001 */ 
         {16, 0xffca}, /* 0xa5:  1111111111001010 */ 
         {16, 0xffcb}, /* 0xa6:  1111111111001011 */ 
         {16, 0xffcc}, /* 0xa7:  1111111111001100 */ 
         {16, 0xffcd}, /* 0xa8:  1111111111001101 */ 
         {16, 0xffce}, /* 0xa9:  1111111111001110 */ 
         {16, 0xffcf}, /* 0xaa:  1111111111001111 */ 
         { 0, 0x0000}, /* 0xab:                   */ 
         { 0, 0x0000}, /* 0xac:                   */ 
         { 0, 0x0000}, /* 0xad:                   */ 
         { 0, 0x0000}, /* 0xae:                   */ 
         { 0, 0x0000}, /* 0xaf:                   */ 
         { 0, 0x0000}, /* 0xb0:                   */ 
         {10, 0x03f9}, /* 0xb1:        1111111001 */ 
         {16, 0xffd0}, /* 0xb2:  1111111111010000 */ 
         {16, 0xffd1}, /* 0xb3:  1111111111010001 */ 
         {16, 0xffd2}, /* 0xb4:  1111111111010010 */ 
         {16, 0xffd3}, /* 0xb5:  1111111111010011 */ 
         {16, 0xffd4}, /* 0xb6:  1111111111010100 */ 
         {16, 0xffd5}, /* 0xb7:  1111111111010101 */ 
         {16, 0xffd6}, /* 0xb8:  1111111111010110 */ 
         {16, 0xffd7}, /* 0xb9:  1111111111010111 */ 
         {16, 0xffd8}, /* 0xba:  1111111111011000 */ 
         { 0, 0x0000}, /* 0xbb:                   */ 
         { 0, 0x0000}, /* 0xbc:                   */ 
         { 0, 0x0000}, /* 0xbd:                   */ 
         { 0, 0x0000}, /* 0xbe:                   */ 
         { 0, 0x0000}, /* 0xbf:                   */ 
         { 0, 0x0000}, /* 0xc0:                   */ 
         {10, 0x03fa}, /* 0xc1:        1111111010 */ 
         {16, 0xffd9}, /* 0xc2:  1111111111011001 */ 
         {16, 0xffda}, /* 0xc3:  1111111111011010 */ 
         {16, 0xffdb}, /* 0xc4:  1111111111011011 */ 
         {16, 0xffdc}, /* 0xc5:  1111111111011100 */ 
         {16, 0xffdd}, /* 0xc6:  1111111111011101 */ 
         {16, 0xffde}, /* 0xc7:  1111111111011110 */ 
         {16, 0xffdf}, /* 0xc8:  1111111111011111 */ 
         {16, 0xffe0}, /* 0xc9:  1111111111100000 */ 
         {16, 0xffe1}, /* 0xca:  1111111111100001 */ 
         { 0, 0x0000}, /* 0xcb:                   */ 
         { 0, 0x0000}, /* 0xcc:                   */ 
         { 0, 0x0000}, /* 0xcd:                   */ 
         { 0, 0x0000}, /* 0xce:                   */ 
         { 0, 0x0000}, /* 0xcf:                   */ 
         { 0, 0x0000}, /* 0xd0:                   */ 
         {11, 0x07f8}, /* 0xd1:       11111111000 */ 
         {16, 0xffe2}, /* 0xd2:  1111111111100010 */ 
         {16, 0xffe3}, /* 0xd3:  1111111111100011 */ 
         {16, 0xffe4}, /* 0xd4:  1111111111100100 */ 
         {16, 0xffe5}, /* 0xd5:  1111111111100101 */ 
         {16, 0xffe6}, /* 0xd6:  1111111111100110 */ 
         {16, 0xffe7}, /* 0xd7:  1111111111100111 */ 
         {16, 0xffe8}, /* 0xd8:  1111111111101000 */ 
         {16, 0xffe9}, /* 0xd9:  1111111111101001 */ 
         {16, 0xffea}, /* 0xda:  1111111111101010 */ 
         { 0, 0x0000}, /* 0xdb:                   */ 
         { 0, 0x0000}, /* 0xdc:                   */ 
         { 0, 0x0000}, /* 0xdd:                   */ 
         { 0, 0x0000}, /* 0xde:                   */ 
         { 0, 0x0000}, /* 0xdf:                   */ 
         { 0, 0x0000}, /* 0xe0:                   */ 
         {16, 0xffeb}, /* 0xe1:  1111111111101011 */ 
         {16, 0xffec}, /* 0xe2:  1111111111101100 */ 
         {16, 0xffed}, /* 0xe3:  1111111111101101 */ 
         {16, 0xffee}, /* 0xe4:  1111111111101110 */ 
         {16, 0xffef}, /* 0xe5:  1111111111101111 */ 
         {16, 0xfff0}, /* 0xe6:  1111111111110000 */ 
         {16, 0xfff1}, /* 0xe7:  1111111111110001 */ 
         {16, 0xfff2}, /* 0xe8:  1111111111110010 */ 
         {16, 0xfff3}, /* 0xe9:  1111111111110011 */ 
         {16, 0xfff4}, /* 0xea:  1111111111110100 */ 
         { 0, 0x0000}, /* 0xeb:                   */ 
         { 0, 0x0000}, /* 0xec:                   */ 
         { 0, 0x0000}, /* 0xed:                   */ 
         { 0, 0x0000}, /* 0xee:                   */ 
         { 0, 0x0000}, /* 0xef:                   */ 
         {11, 0x07f9}, /* 0xf0:       11111111001 */ 
         {16, 0xfff5}, /* 0xf1:  1111111111110101 */ 
         {16, 0xfff6}, /* 0xf2:  1111111111110110 */ 
         {16, 0xfff7}, /* 0xf3:  1111111111110111 */ 
         {16, 0xfff8}, /* 0xf4:  1111111111111000 */ 
         {16, 0xfff9}, /* 0xf5:  1111111111111001 */ 
         {16, 0xfffa}, /* 0xf6:  1111111111111010 */ 
         {16, 0xfffb}, /* 0xf7:  1111111111111011 */ 
         {16, 0xfffc}, /* 0xf8:  1111111111111100 */ 
         {16, 0xfffd}, /* 0xf9:  1111111111111101 */ 
         {16, 0xfffe}, /* 0xfa:  1111111111111110 */ 
         { 0, 0x0000}, /* 0xfb:                   */ 
         { 0, 0x0000}, /* 0xfc:                   */ 
         { 0, 0x0000}, /* 0xfd:                   */ 
         { 0, 0x0000}, /* 0xfe:                   */ 
         { 0, 0x0000} /* 0xff:                   */ 
		}; 
		
//   亮度DC huffman 表										
		int s_lumin_dc[12][2] = { 
         { 2, 0x0000}, /*               00 */ 
         { 3, 0x0002}, /*              010 */ 
         { 3, 0x0003}, /*              011 */ 
         { 3, 0x0004}, /*              100 */ 
         { 3, 0x0005}, /*              101 */ 
         { 3, 0x0006}, /*              110 */ 
         { 4, 0x000e}, /*             1110 */ 
         { 5, 0x001e}, /*            11110 */ 
         { 6, 0x003e}, /*           111110 */ 
         { 7, 0x007e}, /*          1111110 */ 
         { 8, 0x00fe}, /*         11111110 */ 
         { 9, 0x01fe}  /*        111111110 */ 
		}; 	
		work
		{
			S[0].a = s_lumin_ac[4][0];
			S[0].b = s_lumin_ac[4][1];
			S[1].a = s_lumin_dc[4][0];
			S[1].b = s_lumin_dc[4][1];
			//S[2].a = pow(2,2);
			//S[2].b = pow(3,2);
		}
		window
		{
			S tumbling(3);
		}
	};
	Sink(S)
	{
		work
		{	
			println(S[0].a);
			println(S[0].b);
		}
		window
		{
			S sliding(3,1);
		}
	};
}