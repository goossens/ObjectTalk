//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#import <AppKit/AppKit.h>

#include "OtFramework.h"


//
//	OtFramework::fixMenus
//

void OtFramework::fixMenus() {
	// fix menu labels
	NSString* appName = [NSString stringWithCString:"ObjectTalk" encoding:[NSString defaultCStringEncoding]];
	NSMenu* appMenu = [[[NSApp mainMenu] itemAtIndex: 0] submenu];
	[appMenu setTitle: appName];
	[[appMenu itemAtIndex: 0] setTitle: [@"About " stringByAppendingString: appName]];
	[[appMenu itemAtIndex: 6] setTitle: [@"Hide " stringByAppendingString: appName]];
	[[appMenu itemAtIndex: 10] setTitle: [@"Quit " stringByAppendingString: appName]];

	NSMenu* windowMenu = [[[NSApp mainMenu] itemAtIndex: 1] submenu];
	[windowMenu removeItemAtIndex: 0];
}
