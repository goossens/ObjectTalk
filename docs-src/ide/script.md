The ObjectTalk script editor provides a powerful syntax highlighting capability
modeled after a subset of Visual Studio Code. The editor has full undo capabilities
and works with UTF-8 encoded text. The editor can launch ObjectTalk scripts and
has a console for output and highlighting for errors. A visual debugger is also available.

### Additional Features

- Has find/replace user interface and API with full undo/redo.
- Find has options for whole word and/or case-sensitive searches.
- Has optional scrollbar minimap to render cursor, selection and marker locations.
- Provides middle-mouse pan and scroll functions like CAD programs and browsers.
- Provides auto completion for paired glyphs (\[, \{, \(, \", \') (can be turned on and off).
- If auto complete is turned on, accidentally typed closing glyphs are ignored.
- If auto complete is turned on, selections can be surrounded by paired glyphs.
- Allows bracket matching and coloring (similar to Visual Studio Code) to be turned on and off.
- Supports multiple cursors and multiple selections.
- Auto indent with simplified implementation (can be turned on and off).
- Tabs to Space (and visa versa).
- Strip trailing whitespaces on save.
- Whitespace indicators for tabs and spaces (can be turned on and off).
- Shows source code differences since last save.

### Default Keyboard and Mouse Actions

- In the mappings listed below, the following modifier keys are used:
	- Super: this refers to the Control key on MacOS (Dear ImGui reverses Command and Control key on that platform).
	- Ctrl: this refers to the Command key on MacOS and the Control key on Linux and Windows.
	- Alt: this refers to the Option key on MacOS and the Alt key on Linux and Windows.
	- Shift: as you would expect on all platforms.
	- If a keyboard has a left and right version of these modifiers, the meaning is not different.

- Cursor Movements:
	- Single left mouse click moves the cursor.
	- Arrow keys move cursor left, right, up and down.
	- PageUp and PageDown do what you expect.
	- Ctrl-UpArrow and Ctrl-DownArrow move to the start/end of the document.
	- Home and End keys move to the start/end of the line.
	- Holding down the Shift key extends the current selection(s).
	- Holding down the Alt key with the left or right arrow moves a whole word on MacOS.
	- Holding down the Ctrl key with the left or right arrow moves a whole word on Linux and Windows.

- Panning and Scrolling:
	- The text scrolls automatically when you move the cursor through keyboard actions.
	- Mouse actions that extend the selections also apply auto scrolling.
	- The text in the editor can still be scrolled using those bars that were invented in the 1970's.
	- Devices with scroll wheels or those that simulated vertical and horizontal scroll wheels (like a touch pad, a mouse with a builtin touch pad or a pen) can also scroll the text. This is actually implemented in Dear ImGui (and used by the editor) and must be supported by your backend.
	- The middle mouse button on a three-button mouse (or whatever is reported by your OS as a middle mouse button event) enters pan or scroll mode mode depending on the configuration. Pan mode is the default and you can switch this to Scroll mode by calling SetMiddleMouseScrollMode(). Calling SetMiddleMousePanMode() switches it back. The example application uses a menu option to toggle modes.
	- In pan mode, the text is grabbed and dragged as the cursor moves and as long as the middle mouse button is down.
	- When you mouse approaches the edges of the editor window, it enters continuous panning mode and the further you move away form the window, the faster it pans.
	- Panning as described above is typical in CAD or 3D type applications.
	- In scroll mode, you can release the middle mouse button and scroll the text just like you can in some browsers.
	- Scroll mode is ended by any clicking any mouse button.
	- Panning and scrolling operate in opposite directions as they are different paradigms.
	- An optional indicator (default is on) is shown in the center of the editor window when entering pan/scroll mode. If anybody finds this annoying, it can be turned off through an API by calling SetShowPanScrollIndicatorEnabled(false).

- Cursors and Selections:
	- Alt with single left mouse click creates a new cursor on MacOS.
	- Ctrl with single left mouse click creates a new cursor on Linux and Windows.
	- Ctrl-A select all text.
	- Ctrl-D creates a new cursor and selects the next instance of the current selection.
	- Double left mouse clicks on a curly bracket select the content of the relevant block and replaces all previous cursors.
	- Shift + Double left mouse clicks on a curly bracket select the content of the relevant block including the brackets and replaces all previous cursors.
	- Double left mouse clicks not on a bracket or parenthesis, select a word. Adding Shift extends current selection.
	- Triple left mouse clicks select a line. Adding Shift extends current selection.
	- Dragging mouse with left mouse button selects text. Shift extends current selection.
	- Alt-Shift-RightArrow (on MacOS) and Ctrl-Shift-RightArrow (on Linux and Windows) grows all selections to outer blocks. First just the content of the block, than including the curly brackets. Continuously hitting the key combination keeps growing the selections.
	- Alt-Shift-LeftArrow (on MacOS) and Ctrl-Shift-LeftArrow (on Linux and Windows) shrinks all selections to inner blocks. First including the curly brackets, that just the content of the block. Continuously hitting the key combination keeps shrinking the selections.
	- Left mouse clicking or dragging over line numbers select line(s).

- Clipboard Operations:
	- Ctrl-X or Shift-Delete cuts selected text or current line if no selection.
	- Ctrl-C or Ctrl-Insert copies selected text or current line if no selection.
	- Ctrl-V or Shift-Insert pastes clipboard content.
	- Ctrl-Z undoes the last action.
	- Shift-Ctrl-Z or Ctrl-Y redos the last undone action.

- Insert Text:
	- Ctrl-Enter adds line below.
	- Ctrl-Shift-Enter adds line above.

- Delete Text:
	- Backspace deletes character to the left of cursor or deletes selection(s).
	- Delete deletes character to the right of cursor or deletes selection(s).
	- Ctrl-Shift-K deletes all selected lines.

- Text Manipulation:
	- Ctrl-] indents current line or current lines.
	- Ctrl-[ de-indents current line or current lines.
	- Tab with selection indents current line or current lines.
	- Ctrl-UpArrow moves current or all selected lines up.
	- Ctrl-DownArrow moves current or all selected lines down.
	- Ctrl-/ toggles comments if language with single line comments is specified.

- Find & Replace:
	- Ctrl-f opens the find and replace window.
	- Shift-Ctrl-f finds all instances and makes them separate cursors.
	- Ctrl-g finds next instance of search text.

- Font Size:
	- Ctrl-plus increases the font size.
	- Ctrl-minus decreases the font size.

- Other:
	- Insert key toggles between insert (default) and overwrite modes.
