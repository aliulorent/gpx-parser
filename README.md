# GPX Parser and Utilities Web Application

This project was developed for the course CIS2750 - Software Systems Development and Integration at the University of Guelph during the winter-2021 semester.
This course is generally known for being the most demanding course in the Computer Science program at UofG and is commonly referred to as the "Angel of Death".

This winter 2021 project involved us creating a full-stack GPX Parser and Utility application. This application allows users to upload their GPX files and have them display useful information about their GPS data in a clean format. This application also allows them to make changes to their GPX file using the web interface allowing for some easy editing.

## Project Structure Overview

This project was my first ever experience with web development. In this course we learned a great overview of building full stack projects.

Our backend consists of our GPX parser library which was written entirely in C and is stored in the /parser directory.

Our web structure used in this project consists of vanilla Javascript and HTML for the front-end, using jQuery and Express to communicate with our back-end.
We also use MySQL as our database of choice for this project. The web portion of this project can be found in the /public directory for the front-end, and app.js
for our back-end. 