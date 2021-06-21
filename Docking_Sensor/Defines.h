#ifndef DEFINES_H
#define DEFINES_H


#define highWord(w) ((w) >> 16)
#define lowWord(w) ((w) & 0xffff)

//Communication with the robot
#define ADDR_STATUS          0x00		
#define ADDR_CONFIG          0x01		
#define ADDR_CUR_POS         0x02		
#define ADDR_REF_POS         0x03		
#define ADDR_LIMITS          0x04		
#define ADDR_SHARP           0x05		
#define ADDR_RELAY           0x06		
#define ADDR_REQ             0x07		
#define ADDR_BLUEBOTS_VEL    125		
#define ADDR_BLUEBOTS_BUTTON 127		

#endif
