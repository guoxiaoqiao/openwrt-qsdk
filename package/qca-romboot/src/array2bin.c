/*
 * Copyright (c) 2013 Qualcomm Atheros, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>

static int is_little_endian(void)
{
    unsigned int word = 0x1;
    unsigned char *byte = (unsigned char *) &word ;

    return (byte[0] ? 1 : 0);
}

int main(int argc, char *argv[])
{
    FILE *in;
    FILE *out;
    char *infname = argv[1];
    char *outfname = argv[2];
    unsigned char c;
    unsigned char tempArray[9] = {0};
    unsigned int tempInt = 0;
    unsigned int dataInt = 0;
    unsigned int hex_digit, i;
    unsigned int convert_base[8] = {268435456, 16777216, 1048576, 65536, 4096, 256, 16, 1};
   
    if ((infname == NULL) || (outfname == NULL)) {
        printf("input name error\n");
        return -1;
    }

    if ((in = fopen(infname, "rb")) == NULL) {
        printf("open input file fails\n");
        return -1;
    }

    if ((out = fopen(outfname, "wb")) == NULL) {
        printf("open output file fails\n");
        return -1;
    }

    while (fread(&c, sizeof(unsigned char), 1, in)) {
        if (c == '0') {
		    fread(&c, sizeof(unsigned char), 1, in);
		    if (c == 'x') {
			    fread(tempArray, sizeof(unsigned char), 8, in);
			    tempArray[8] = 0;
			    for (i = 8; i > 0; i--) {
				    if ((tempArray[i-1] > 47) && (tempArray[i-1] < 58)) {
					    hex_digit = tempArray[i-1] - 48;
				    } else if ((tempArray[i-1] > 64) && (tempArray[i-1] < 71)) {
					    hex_digit = tempArray[i-1] - 55;
				    } else {
        				printf("input name error\n");
        				return -1;
				    }   
				    tempInt += (hex_digit * convert_base[i-1]);
			    }   

			    if(is_little_endian()) {
				    for (i = 0; i < 4; i++) {
					    *(((char *)&dataInt)+i) = *(((char *)&tempInt)+(3-i));
				    }
			    } else {
				    dataInt = tempInt;
			    }

			    fwrite((char *)&dataInt, sizeof(unsigned char), 4, out);
			    tempInt = 0;
			    dataInt = 0;
		    }
	    }
    }

    fclose(in);
    fclose(out);
        
    return 0;
}
