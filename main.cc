// -----------------------------------------------------
/** @file main.cc
 * This is the program that you have to finish.
 */
// -----------------------------------------------------

// Our own includes
#include "ansi.h"		// for: ansi color code strings
#include "asserts.h"	// for: notreached() e.a.
#include "unix_error.h"	// for: the unix_error() exception class
#include "cstr.h"		// for: the cstr() wrapper and it's output operator

// C/C++/STL specific
#include <iostream>	    // for: std::cout, std::endl
#include <ctime>		// for: ctime()
#include <cstdlib>		// for: EXIT_SUCCESS, EXIT_FAILURE
#include <unistd.h>		// for: close() etc
#include <fcntl.h>		// for: O_WRONLY etc

// Our own classes
#include "Device.h"		// the "device driver"
#include "Block.h"		// the "data blocks"

void	printRootInode(Device&);
void	printSuperBlock(Device&);
void	charter(const char*);

// ================================================================


// TODO: write all the functions etc you need for this assignment
void	log( const char* floppie )
{
    std::cout << "Opening device '" << floppie << "'" << std::endl;
    Device  device(floppie);	// A device instance
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    printSuperBlock(device);
    printRootInode(device);


}

// ================================================================

// Main is just the TUI
int  main(int argc, const char* argv[])
{
	try {
// Note: To get your output into file 'log.txt', change the 0 below into a 1
#if	0
		std::cerr << "Sending output to log.txt" << std::endl;
		close(1);
		if( open("log.txt", O_WRONLY|O_TRUNC|O_CREAT, 0666) < 0)
			throw unix_error("log.txt");
#endif
		// Tell cout to add a 0 or 0x prefix when printing in non-decimal notation
		std::cout << std::showbase;

		// Pass the given parameter or use the default filename ?
		log((argc > 1) ? argv[1] : "floppie.img");



		return EXIT_SUCCESS;
	} catch(const unix_error& e) {
		std::cerr << AC_RED "SYSTEM: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(const std::exception& e) {
		std::cerr << AC_RED "OOPS: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << AC_RED "OOPS: something went wrong" AA_RESET << std::endl;
		return EXIT_FAILURE;
	}
}

void printSuperBlock(Device& device)
{
    //get the SuperBlock of the given divice
	Block*  sp = device.getBlock(SUPERB);

    std::cout << "Log of superblock on volume: " << cstr(sp->u.fs.s_fname,6) << "." << cstr(sp->u.fs.s_fpack,6)  << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "userdata area starts in block: " << sp->u.fs.s_isize << std::endl;
    std::cout << "number of blocks on this volume is: " << sp->u.fs.s_fsize << std::endl << std::endl;
    std::cout << "number of freeblocks in free[] is: " << sp->u.fs.s_nfree << " (max " << NICFREE << ")"<< std::endl;

    ///free blocks
    std::cout << "======================================LIST======================================" << std::endl;
    for(unsigned  i = 0; i < sp->u.fs.s_nfree; ++i) {
		std::cout << sp->u.fs.s_free[i] << " ";
	}
    std::cout << std::endl;
    std::cout << "====================================END LIST====================================" << std::endl << std::endl;

    std::cout << "number of free inodes in inode[] is: " <<  sp->u.fs.s_ninode << " (max " << NICINOD << ")"<< std::endl;

    ///free inodes
    std::cout << "======================================LIST======================================" << std::endl;
    for(unsigned  i = 0; i < sp->u.fs.s_ninode; ++i) {
		std::cout << sp->u.fs.s_inode[i] << " ";
	}
    std::cout << std::endl;
    std::cout << "====================================END LIST====================================" << std::endl << std::endl;

    ///locks
    //freelist
    std::cout << "freelist lock flag is: ";
    if(sp->u.fs.s_flock) {
        std::cout << "set";
    } else {
        std::cout << "not set";
    }
    std::cout << std::endl;

    //i-list
    std::cout << "i-list lock flag is: ";
    if(sp->u.fs.s_ilock) {
        std::cout << "set";
    } else {
        std::cout << "not set";
    }
    std::cout << std::endl;

    //modified
    std::cout << "superblock-modified is: ";
    if(sp->u.fs.s_fmod) {
        std::cout << "set";
    } else {
        std::cout << "not set";
    }
    std::cout << std::endl;

    //read-only
    std::cout << "read-only flag is: ";
    if(sp->u.fs.s_ronly) {
        std::cout << "set";
    } else {
        std::cout << "not set";
    }
    std::cout << std::endl;

    std::cout << std::endl;
    ///update time
    time_x x = sp->u.fs.s_time;
    time_t t = (time_t) x;
    std::cout << "last update time was: " << ctime(&t);

    ///total free
    std::cout << std::endl;
    //blocks
    std::cout << "total number of free blocks is: "<< sp->u.fs.s_tfree << std::endl;

    //inodes
    std::cout << "total number of free inodes is: "<< sp->u.fs.s_tinode << std::endl;

    ///interleave
    std::cout << std::endl;
    std::cout << "Interleave factors are: m=" << sp->u.fs.s_m <<" and n="<< sp->u.fs.s_n << std::endl;



    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------------------------------" << std::endl;

    std::cout << std::endl << std::endl;

//   Note 2:
// When the definition for time_t on your system is not compatible
// with the definition for time_x here, you can not directly use the
// ctime(...) function to convert a timestamp to human readable text.
// In that case you have to use a two-step approach:
//  1: assign the time_x value to a time_t variable,
//  2: use ctime() to convert that time_t value to text.


	//	printf("printf     fs.s_isize=%d fs.s_fsize=%d\n",
	//		sp->u.fs.s_isize, sp->u.fs.s_fsize);
	// See Also: man 3 printf
	// ... or use the iostream operators from C++
	std::cout << "fs.s_isize=" << sp->u.fs.s_isize
			  << " fs.s_fsize=" << sp->u.fs.s_fsize << std::endl;

	// The superblock-last-update timestamp can be
	// converted to human readable format with ctime()
	// (which always adds a newline character!) ...
	//time_t* t = reinterpret_cast<time_t*>(&sp->u.fs.s_time);
	//std::cout << "fs.s_time=" << ctime(t);   // see: man 3 ctime
	// ... or use printf and limit the output to 24 chars
	//printf("fs.s_time=%.24s\n", ctime(t));	// see: man 3 ctime
	// Note: The time shown may differ from the given dump.txt file
	//		 because of daylight savings time and different timezones.
	//		 Internally all times are expressed in GMT.
	// Note: When on your system a time_t is 64 bit, you can not use
	// 		 ctime(&...) directly. Instead you have to use a little
	// 		 work-around. First assign the 32 bit time_x timestamp
	// 		 to a 64 bit time_t temporary:
	// 		 	time_t  x = sp->u.fs.s_time;
	// 		 then use ctime on that temporary
	// 		 	... ctime(&x) ...

	// Because on the actual filesystem "strings" are not
	// guarantied to be null-terminated, simply printing
	// them you may risk printing your entire memory until
	// we somewhere run into a 0-byte.
	// To handle this problem a special C-string wrapper class
	// 		cstr(const char[] text, unsigned maxlen)
	// with a suitable output operator is provided ...
	std::cout << "fs.s_fname=" << cstr(sp->u.fs.s_fname,6)
			  << ", fs.s_fpack=" << cstr(sp->u.fs.s_fpack,6) << std::endl;
	// .. or use printf again and tell it to stop after N characters.
	//printf("fs.s_fname=%.6s, fs.s_fpack=%.6s\n",
	//		sp->u.fs.s_fname, sp->u.fs.s_fpack);

	sp->release();	// We no longer need the super block

}

void	printRootInode(Device& device)
{
	// - - - - - - - - - - - - -
	// read INODE's from disk
	// Also see: e7ino.h
	// - - - - - - - - - - - - -

	// Fetch the block containing the root inode
	Block*  ip = device.getBlock( itod(ROOTINO) );

	// Going to use the u.dino[] view on the block.

	// Use a pointer variable as a shortcut to the wanted dinode in this block
	dinode*	dp = &ip->u.dino[ itoo(ROOTINO) ];

	// Print the inode type + protection flags
	// If (di_mode == 0) then this inode is not used

	//printf("inode %d, mode=%#o (expect 040777)\n", ROOTINO, dp->di_mode);
	// or
	std::cout << "inode " << ROOTINO << ", mode="
			  // switching between octal/decimal notation
			  << std::oct << dp->di_mode << std::dec
			  << " (expect 040777)" << std::endl;

	// Verify this is indeed a directory inode using a "bit-mask test"
	if((dp->di_mode & X_IFMT) == X_IFDIR) {
		printf(AC_GREEN	"Good: it is a directory\n"		AA_RESET);
	} else {
		printf(AC_RED	"Oops: it is not a directory\n"	AA_RESET);
	}

	// Convert the 13, 24-bits, blocknumbers in the inode
	// to normal 32-bit daddr_x values (only valid for DIR or REG type!)
	daddr_x  diskaddrs[NADDR];				// provide some room to store those 13 blocknumbers
	Block::l3tol(diskaddrs, dp->di_addr);	// the data convertion routine

	std::cout << "diskaddr: ";
	for(auto addr : diskaddrs) {			// A C++ "range-base-for-loop"
        std::cout << addr << " ";
		//printf(" %d", addr);
	}
	std::cout << std::endl;

	ip->release();	// We no longer need this inode block
}




