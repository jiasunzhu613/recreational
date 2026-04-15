# A pseudo-terminal recreation

Pseudo-terminal or PTY are character device pairs implemented as a master and a slave file, providing a bidirectional communication channel that allows one program to control another, like in an actual physical terminal. 

The master end of a PTY is used by a controlling application (like terminal emulators) and the slave end of a PTY is used to handle actual logic and expects terminal behaviours like line buffering and signal handling. 

A PTY is different from a generic pipe as PTYs implement "line discipline" (which is managed by the kernel) handling things like special characters (such as ^C for signalling), line buffering, and window resizing. 

### Architecture (of this paricular example)
Though PTYs can be applied to many applications, one of its common use cases is to display and handle the interactions in a shell (command interpreter) such as `sh` or `bash`.

 ┌─────────────────────────────────────────────────────────┐ 
 │ ┌─────────────────────┐           ┌───────────────────┐ │ 
 │ │                     │           │                   │ │ 
 │ │                     │           │                   │ │ 
 │ │                     │    fork() │                   │ │ 
 │ │  user application   │───── ────►│     /bin/bash     │ │ 
 │ │                     │           │                   │ │ 
 │ │                     │           │                   │ │ 
 │ │                     │           │                   │ │ 
 │ └─────────────────────┘           └───────────────────┘ │ 
 │        │        ▲                       │     ▲         │ 
 │        │        │                       │     │         │ 
 └─────────────────────────────────────────────────────────┘ 
          │        │                       │     │           
       write()   read()                  stdout  │           
          │        │                       │    stdin        
 Kernel   │        │                       │     │           
┌─────────▼────────┼───────────────────────▼─────┼──────────┐
│  ┌───────────────┴─────┐          ┌────────────┴───────┐  │
│  │                     │          │                    │  │
│  │                     │          │                    │  │
│  │                     ├─────────►│                    │  │
│  │  PTM ( master end)  │          │   PTS (slave end)  │  │
│  │                     │◄─────────┤                    │  │
│  │                     │          │                    │  │
│  │                     │          │                    │  │
│  └─────────────────────┘          └────────────────────┘  │
└───────────────────────────────────────────────────────────┘
(Made with: https://asciiflow.com/#/)

### TODO:
- [ ] REPL header printing
- [ ] add ioctl management for terminal window sizing so that text doesnt get cut off

### Learing Resources
General overview of terminals + some history: https://www.linusakesson.net/programming/tty/
Brief on line discipline (post processing done by the kernel?): https://en.wikipedia.org/wiki/Line_discipline