//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2021 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <Cocoa/Cocoa.h>

#include "application.h"


//
//	OtApplicationClass::fixMenuLabels
//

void OtApplicationClass::fixMenuLabels(const std::string& name) {
	// fix menu labels
	NSString* appName = [NSString stringWithCString:name.c_str() encoding:[NSString defaultCStringEncoding]];
	NSMenu* menu = [[[NSApp mainMenu] itemAtIndex: 0] submenu];
	[menu setTitle: appName];
	[[menu itemAtIndex: 0] setTitle: [@"About " stringByAppendingString:appName]];
	[[menu itemAtIndex: 8] setTitle: [@"Quit " stringByAppendingString:appName]];
}
