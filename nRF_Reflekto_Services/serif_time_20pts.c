#include "nrf_font.h"

// 
//  Font data for Microsoft Sans Serif 20pt
// 

// Character bitmaps for Microsoft Sans Serif 20pt
const uint8_t microsoftSansSerif_20ptBitmaps[] = 
{
	// @0 '0' (13 pixels wide)
	0x0F, 0x80, //     #####    
	0x3F, 0xE0, //   #########  
	0x30, 0x60, //   ##     ##  
	0x60, 0x30, //  ##       ## 
	0x60, 0x30, //  ##       ## 
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0x60, 0x30, //  ##       ## 
	0x60, 0x30, //  ##       ## 
	0x30, 0x60, //   ##     ##  
	0x3F, 0xE0, //   #########  
	0x0F, 0x80, //     #####    

	// @38 '1' (7 pixels wide)
	0x06, //      ##
	0x06, //      ##
	0x0E, //     ###
	0x1E, //    ####
	0xFE, // #######
	0xF6, // #### ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##
	0x06, //      ##

	// @57 '2' (13 pixels wide)
	0x0F, 0xC0, //     ######   
	0x3F, 0xE0, //   #########  
	0x70, 0x70, //  ###     ### 
	0x60, 0x38, //  ##       ###
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0x00, 0x18, //            ##
	0x00, 0x38, //           ###
	0x00, 0x30, //           ## 
	0x00, 0x60, //          ##  
	0x01, 0xC0, //        ###   
	0x03, 0x80, //       ###    
	0x07, 0x00, //      ###     
	0x1C, 0x00, //    ###       
	0x38, 0x00, //   ###        
	0x70, 0x00, //  ###         
	0x60, 0x00, //  ##          
	0xFF, 0xF8, // #############
	0xFF, 0xF8, // #############

	// @95 '3' (13 pixels wide)
	0x1F, 0x80, //    ######    
	0x3F, 0xE0, //   #########  
	0x70, 0x60, //  ###     ##  
	0xE0, 0x30, // ###       ## 
	0xC0, 0x30, // ##        ## 
	0x00, 0x30, //           ## 
	0x00, 0x30, //           ## 
	0x00, 0xE0, //         ###  
	0x07, 0xC0, //      #####   
	0x07, 0xE0, //      ######  
	0x00, 0x70, //          ### 
	0x00, 0x38, //           ###
	0x00, 0x18, //            ##
	0x00, 0x18, //            ##
	0xC0, 0x18, // ##         ##
	0xE0, 0x38, // ###       ###
	0x70, 0x70, //  ###     ### 
	0x3F, 0xE0, //   #########  
	0x1F, 0x80, //    ######    

	// @133 '4' (14 pixels wide)
	0x00, 0x60, //          ##   
	0x00, 0xE0, //         ###   
	0x00, 0xE0, //         ###   
	0x01, 0xE0, //        ####   
	0x03, 0x60, //       ## ##   
	0x06, 0x60, //      ##  ##   
	0x06, 0x60, //      ##  ##   
	0x0C, 0x60, //     ##   ##   
	0x18, 0x60, //    ##    ##   
	0x18, 0x60, //    ##    ##   
	0x30, 0x60, //   ##     ##   
	0x60, 0x60, //  ##      ##   
	0xC0, 0x60, // ##       ##   
	0xFF, 0xFC, // ##############
	0xFF, 0xFC, // ##############
	0x00, 0x60, //          ##   
	0x00, 0x60, //          ##   
	0x00, 0x60, //          ##   
	0x00, 0x60, //          ##   

	// @171 '5' (13 pixels wide)
	0x3F, 0xF0, //   ########## 
	0x3F, 0xF0, //   ########## 
	0x30, 0x00, //   ##         
	0x30, 0x00, //   ##         
	0x30, 0x00, //   ##         
	0x60, 0x00, //  ##          
	0x67, 0x80, //  ##  ####    
	0x7F, 0xE0, //  ##########  
	0x78, 0x70, //  ####    ### 
	0x60, 0x30, //  ##       ## 
	0x00, 0x18, //            ##
	0x00, 0x18, //            ##
	0x00, 0x18, //            ##
	0x00, 0x18, //            ##
	0xC0, 0x18, // ##         ##
	0xE0, 0x30, // ###       ## 
	0x70, 0x70, //  ###     ### 
	0x3F, 0xE0, //   #########  
	0x1F, 0x80, //    ######    

	// @209 '6' (13 pixels wide)
	0x0F, 0xC0, //     ######   
	0x1F, 0xE0, //    ########  
	0x38, 0x70, //   ###    ### 
	0x70, 0x18, //  ###       ##
	0x60, 0x18, //  ##        ##
	0xE0, 0x00, // ###          
	0xC0, 0x00, // ##           
	0xCF, 0x80, // ##  #####    
	0xDF, 0xE0, // ## ########  
	0xF0, 0x70, // ####     ### 
	0xC0, 0x38, // ##        ###
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0x60, 0x18, //  ##        ##
	0x60, 0x30, //  ##       ## 
	0x38, 0x70, //   ###    ### 
	0x1F, 0xE0, //    ########  
	0x0F, 0x80, //     #####    

	// @247 '7' (13 pixels wide)
	0xFF, 0xF8, // #############
	0xFF, 0xF8, // #############
	0x00, 0x38, //           ###
	0x00, 0x30, //           ## 
	0x00, 0x30, //           ## 
	0x00, 0x60, //          ##  
	0x00, 0x60, //          ##  
	0x00, 0xC0, //         ##   
	0x00, 0xC0, //         ##   
	0x01, 0x80, //        ##    
	0x01, 0x80, //        ##    
	0x03, 0x00, //       ##     
	0x03, 0x00, //       ##     
	0x07, 0x00, //      ###     
	0x06, 0x00, //      ##      
	0x06, 0x00, //      ##      
	0x0C, 0x00, //     ##       
	0x0C, 0x00, //     ##       
	0x18, 0x00, //    ##        

	// @285 '8' (13 pixels wide)
	0x0F, 0x80, //     #####    
	0x3F, 0xE0, //   #########  
	0x70, 0x70, //  ###     ### 
	0x60, 0x30, //  ##       ## 
	0x60, 0x30, //  ##       ## 
	0x60, 0x30, //  ##       ## 
	0x70, 0x70, //  ###     ### 
	0x38, 0xE0, //   ###   ###  
	0x1F, 0xC0, //    #######   
	0x1F, 0xC0, //    #######   
	0x38, 0xE0, //   ###   ###  
	0x60, 0x30, //  ##       ## 
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xE0, 0x38, // ###       ###
	0x70, 0x70, //  ###     ### 
	0x3F, 0xE0, //   #########  
	0x1F, 0xC0, //    #######   

	// @323 '9' (13 pixels wide)
	0x0F, 0x80, //     #####    
	0x3F, 0xC0, //   ########   
	0x70, 0xE0, //  ###    ###  
	0x60, 0x30, //  ##       ## 
	0xC0, 0x30, // ##        ## 
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xC0, 0x18, // ##         ##
	0xE0, 0x18, // ###        ##
	0x70, 0x78, //  ###     ####
	0x3F, 0xD8, //   ######## ##
	0x0F, 0x98, //     #####  ##
	0x00, 0x18, //            ##
	0x00, 0x38, //           ###
	0xC0, 0x30, // ##        ## 
	0xC0, 0x70, // ##       ### 
	0x70, 0xE0, //  ###    ###  
	0x3F, 0xC0, //   ########   
	0x1F, 0x80, //    ######    

	// @361 ':' (2 pixels wide)
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0x00, //   
	0xC0, // ##
	0xC0, // ##
	0xC0, // ##
};

// Character descriptors for Microsoft Sans Serif 20pt
// { [Char width in bits], [Offset into microsoftSansSerif_20ptCharBitmaps in bytes] }
const FONT_CHAR_INFO microsoftSansSerif_20ptDescriptors[] = 
{
	{13, 0}, 		// 0 
	{7, 38}, 		// 1 
	{13, 57}, 		// 2 
	{13, 95}, 		// 3 
	{14, 133}, 		// 4 
	{13, 171}, 		// 5 
	{13, 209}, 		// 6 
	{13, 247}, 		// 7 
	{13, 285}, 		// 8 
	{13, 323}, 		// 9 
	{2, 361}, 		// : 
};

// Font information for Microsoft Sans Serif 20pt
const FONT_INFO microsoftSansSerif_20ptFontInfo =
{
	19, //  Character height
	'0', //  Start character
	':', //  End character
	2, //  Width, in pixels, of space character
	microsoftSansSerif_20ptDescriptors, //  Character descriptor array
	microsoftSansSerif_20ptBitmaps, //  Character bitmap array
};
