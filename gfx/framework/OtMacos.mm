//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2025 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#import <objc/runtime.h>

#import <AppKit/AppKit.h>
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/CAMetalLayer.h>

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


//
//	OtFramework::createMetalLayer
//

void OtFramework::createMetalLayer() {
	// get content view
	NSWindow* nsWindow = (NSWindow*) nativeWindowHandle;
	NSView* contentView = [nsWindow contentView];

	// create metal layer
	[contentView setWantsLayer: YES];
	CAMetalLayer* metalLayer = [CAMetalLayer layer];
	[contentView setLayer:metalLayer];

	// store metal layer for use in BGFX
	nativeWindowHandle = (void*) metalLayer;
}
