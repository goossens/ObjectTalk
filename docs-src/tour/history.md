In the 80s, I was developing military command and control systems in an
international setting. As was true for most software development at that
time, memory and disk capacities were a scarce commodity and optimization
strategies were always important. In the system I worked on, our custom
Operating System and real-time applications were written in assembler.
You can probably imagine that it became a rats nest over time.
In one case, a programmer was so smart that he patched jump instructions
to "efficiently" create a switch/case type statement. As you probably
already expect, this was not maintainable and pre-deployment testing
started to take way too much time.

Luckily, our assembler had a simple pre-processor that allowed macros
and I developed a set of macros for structured programming in assembler.
This stopped all hacking and the code became more readable.
Control structures and program flows where more visible and thanks to
a coding style guide, all of our products became more stable.

At the same time, we also started to receive "desktop" UNIX computers
which I initially used for software development and code verification.
This also opened the door for higher level language development and
I write my first compiler that took higher level code and translated it
to the assembler required for our mainframe. This worked very well and
my interest in computer language development was born.

In the late 80s, early 90s, I moved to the US and was in charge of a
team of software developers. Of the 20 people or so, five were really
good and could handle the demands posed on them by our HPUX machines loaded
with X Windows, Motif, Oracle DB and a number of low level packages.
The others struggled with the complexity of the C code and certainly the
APIs to Motif and Oracle. In order to make the team more effective, I
developed a scripting language called Talk that was easy to learn,
abstracted away all the Motif and Oracle complexities and ensured that
GUI guidelines were enforces and bad coding habits like endless "jumps",
"breaks" and "continues" were no longer possible. This became very
successful and 30+ years later the language is still in use.

Off course you can ask why we didn't use an existing language? Well, in
those early days, we just couldn't find one that met our criteria.
Existing higher level languages like Cobol, Fortran, Pascal and Ada were
too complex and creating binding to other packages was not easy. If we had
done this later, we might have picked Python or Lua but those languages
have also become complex (but at least, bindings are easy).

As I started to move up the management chain, I got further and further
away from programing and at the end, I spent more time in international
politics than thinking about technology or innovations. So to challenge
my brain, I continued tinkering with a number of things in my private
time and that's how ObjectTalk was born. Initially, I made Talk
object-oriented and continued to add features. This was a really
slow process as family and work demands did not allow a lot of time to
work on this.

Well, as you can read in my biography, I'm now retired and I've decided
to dust a lot of this stuff off and make ObjectTalk available as Open Source.
