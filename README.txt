STORM DRAIN TELEMETRY AND ASSET MANAGEMENT SYSTEM
Semester 3 C++ PBL | Readable source-code edition

QUICK START (WINDOWS)
1. Extract this ZIP to a normal folder (do not run it from inside the ZIP).
2. Install MSYS2 with the UCRT64 GCC compiler if it is not already installed:
   https://www.msys2.org/
3. Open the MSYS2 UCRT64 terminal and run:
   pacman -S --needed mingw-w64-ucrt-x86_64-gcc
4. Close the terminal. Double-click RUN_WINDOWS.bat in the extracted project folder.
   The script compiles the C++ backend and starts it automatically.
5. Keep the black terminal window open while using the website.
6. Open these URLs in your browser:
   Public dashboard: http://localhost:8088/
   Admin portal:     http://localhost:8088/admin.html

DEMO ADMIN LOGIN
Username: admin
Password: Drain@2026
This is a classroom demo credential. Do not use it for a public deployment.

WHAT IS INCLUDED
- C++17 HTTP server and Open-Meteo weather API client.
- Public dashboard and admin portal (HTML, CSS, JavaScript).
- Graph with BFS traversal for illustrative zone connectivity.
- Hash table using linear probing for site records.
- Circular queue for alert IDs awaiting review.
- Max-heap for selecting the highest weather-risk score.
- Doubly linked list for maintenance activity history.
- CSV file for maintenance records.
- Comments and readable formatting intended for a Semester 3 code walkthrough.

IMPORTANT PROJECT LIMITATION
This is a software-only prototype. It fetches weather observations from Open-Meteo; it does not read physical drain sensors, measure actual drain water depth, detect a blockage, or provide a certified flood forecast. The risk score is a demonstration heuristic and must not be used for emergency decisions. The example sites and graph connections are illustrative.

HOW THE DATA STRUCTURES ARE USED
1. Graph: stores directed connections between configured locations; BFS traverses reachable locations.
2. Hash table: stores the latest record for each site and updates a record by its ID.
3. Circular queue: tracks alert IDs awaiting review in FIFO order and avoids duplicate queued IDs.
4. Max-heap: keeps the highest risk score at the root for quick access.
5. Doubly linked list: stores maintenance records and traverses backward to show newest first.

API ROUTES
GET  /api/health
GET  /api/public/overview
GET  /api/public/sites
POST /api/admin/login
GET  /api/admin/sites            Requires X-Auth-Token
GET  /api/admin/alerts           Requires X-Auth-Token
GET  /api/admin/maintenance      Requires X-Auth-Token
GET  /api/admin/graph            Requires X-Auth-Token
POST /api/admin/maintenance      Requires X-Auth-Token

WEATHER UPDATE BEHAVIOUR
The backend attempts a refresh at startup and then every 10 minutes. The browser refreshes its dashboard view periodically. Internet access is required for current weather observations. If an API request fails, the dashboard should show the error/stale status rather than treating the reading as a live drain measurement.

RISK SCORE
The current demonstration score uses rainfall, precipitation, humidity, and wind values, capped at 100. The score is an unvalidated heuristic, not a hydrological model.

TROUBLESHOOTING
- “g++ was not found”: install GCC using the MSYS2 UCRT64 steps above.
- Build errors: make sure you installed the UCRT64 compiler, not only the MSYS2 base package.
- “Could not bind port 8088”: close any other app/server using port 8088 and run the BAT file again.
- Weather data unavailable: check internet access; API availability may vary. The app may still start while reporting that live weather could not be fetched.
- Admin login fails: use the demo credentials unless you set STORM_ADMIN_USER and STORM_ADMIN_PASSWORD environment variables.
- Always start the server through RUN_WINDOWS.bat or run the executable from the backend folder, because the app uses relative paths for frontend and data files.

SOURCE REFERENCE
Open-Meteo documentation: https://open-meteo.com/en/docs
