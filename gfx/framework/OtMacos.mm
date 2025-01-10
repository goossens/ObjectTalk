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
//	GLFWCustomDelegate
//

//
//	This code is required to catch application: (NSApplication *) sender openFile(s) events
//	raised by the Finder when a user double clicks on a document
//
//	See: https://github.com/glfw/glfw/issues/1024
//

NS_ASSUME_NONNULL_BEGIN
@interface GLFWCustomDelegate : NSObject
+ (void) load; // load is called before even main() is run (as part of objc class registration)
@end
NS_ASSUME_NONNULL_END

@implementation GLFWCustomDelegate
+ (void) load {
	static dispatch_once_t onceToken;

	dispatch_once(&onceToken, ^ {
		Class cls = objc_getClass("GLFWApplicationDelegate");
		[GLFWCustomDelegate swizzle:cls src:@selector(application:openFile:) tgt:@selector(swizzleApp:openFile:)];
		[GLFWCustomDelegate swizzle:cls src:@selector(application:openFiles:) tgt:@selector(swizzleApp:openFiles:)];
	});
}

+ (void) swizzle: (Class) originalClass src: (SEL) originalSelector tgt: (SEL) targetSelector {
	Class targetClass = [GLFWCustomDelegate class];
	Method originalMethod = class_getInstanceMethod(originalClass, originalSelector);
	Method swizzledMethod = class_getInstanceMethod(targetClass, targetSelector);

	BOOL didAddMethod = class_addMethod(
		originalClass,
		originalSelector,
		method_getImplementation(swizzledMethod),
		method_getTypeEncoding(swizzledMethod));

	if (didAddMethod) {
		class_replaceMethod(
			originalClass,
			targetSelector,
			method_getImplementation(originalMethod),
			method_getTypeEncoding(originalMethod));

	} else {
		method_exchangeImplementations(originalMethod, swizzledMethod);
	}
}

- (BOOL) swizzleApp: (NSApplication *)sender openFile: (NSString *) filename {
	OtFramework::addStartupFile(filename.UTF8String);
	return YES;
}

- (void) swizzleApp: (NSApplication *)sender openFiles: (NSArray<NSString*> *) filenames {
	for (NSString* filename in filenames) {
		OtFramework::addStartupFile(filename.UTF8String);
    }
}

@end


//
//	OtFramework::addStartupFile
//

void OtFramework::addStartupFile(const char* filename) {
	filesToOpen.emplace_back(filename);
}


//
//	OtFramework::fixMenuLabels
//

void OtFramework::fixMenuLabels() {
	// fix menu labels
	NSString* appName = [NSString stringWithCString:"ObjectTalk" encoding:[NSString defaultCStringEncoding]];
	NSMenu* menu = [[[NSApp mainMenu] itemAtIndex: 0] submenu];
	[menu setTitle: appName];
	[[menu itemAtIndex: 0] setTitle: [@"About " stringByAppendingString:appName]];
	[[menu itemAtIndex: 8] setTitle: [@"Quit " stringByAppendingString:appName]];
}


//
//	OtFramework::createMetalLayer
//

void OtFramework::createMetalLayer() {
	// get content view
	NSWindow* nsWindow = (NSWindow*) nativeDisplayHandle;
	NSView* contentView = [nsWindow contentView];

	// create metal layer
	[contentView setWantsLayer: YES];
	CAMetalLayer* metalLayer = [CAMetalLayer layer];
	[contentView setLayer:metalLayer];

	// store metal layer for use in BGFX
	nativeDisplayHandle = (void*) metalLayer;
}
