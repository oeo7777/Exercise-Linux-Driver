#ifndef _IOC_TEST_H
#define _IOC_TEST_H

#define IOC_MAGIC                  0x13  //'k'
#define SETNUM  _IOW(IOC_MAGIC, 15 , int)
#define GETNUM 	_IOR(IOC_MAGIC, 16 , int)
#define XNUM 	_IOWR(IOC_MAGIC,17 , int)
#define IOC_MAXNR 20


#endif