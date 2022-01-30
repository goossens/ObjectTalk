//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2022 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#import <Cocoa/Cocoa.h>

#include "OtFramework.h"


//
//	OtFrameworkClass::fixMenuLabels
//

void OtFrameworkClass::fixMenuLabels() {
	// fix menu labels
	NSString* appName = [NSString stringWithCString:"ObjectTalk" encoding:[NSString defaultCStringEncoding]];
	NSMenu* menu = [[[NSApp mainMenu] itemAtIndex: 0] submenu];
	[menu setTitle: appName];
	[[menu itemAtIndex: 0] setTitle: [@"About " stringByAppendingString:appName]];
	[[menu itemAtIndex: 8] setTitle: [@"Quit " stringByAppendingString:appName]];
}
