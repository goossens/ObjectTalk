//	ObjectTalk Scripting Language
//	Copyright (c) 1993-2023 Johan A. Goossens. All rights reserved.
//
//	This work is licensed under the terms of the MIT license.
//	For a copy, see <https://opensource.org/licenses/MIT>.


//
//	Include files
//

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

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


//
//	OtFrameworkClass::createMetalLayer
//

void OtFrameworkClass::createMetalLayer() {
	// get content view
	NSWindow* nsWindow = (NSWindow*) nativeDisplayHandle;
	NSView *contentView = [nsWindow contentView];

	// create metal layer
	[contentView setWantsLayer: YES];
	CAMetalLayer* metalLayer = [CAMetalLayer layer];
	[contentView setLayer:metalLayer];

	// store metal layer for use in BGFX
	nativeDisplayHandle = (void*) metalLayer;
}
