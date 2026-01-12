# Online Learning Platform

_(Cloud, DevOps & Systems Engineering Project)_

![System Overview](docs/images/system-overview.png)

A cloud-native **online learning platform** designed and implemented with a strong focus on **systems architecture, containerization, CI/CD automation, cloud deployment, and observability**.

---

## Overview

This project is an **E-learning web platform** that allows users to browse courses and access video-based learning content.

The system is built as a **containerized backend written in C++**, deployed on AWS using modern DevOps practices.  
It integrates **secure object storage, automated deployment pipelines, and monitoring with alerting**.

The primary goal of this project is to demonstrate **end-to-end Cloud & DevOps skills**, not just application development.

---

## System Architecture

The system follows a modular and cloud-oriented architecture:

- A **static frontend** built with HTML/CSS
- A **C++ backend API** implemented using the Crow framework
- Secure video access via **S3 Presigned URLs**
- Containerized deployment using **Docker + AWS ECS**
- Automated CI/CD with **GitHub Actions**
- Monitoring and alerting with **Prometheus, Grafana, and Telegram**

### Runtime Flow

1. Users access the platform via a web browser
2. The frontend sends HTTP requests to the backend API
3. The backend processes business logic and data structures
4. Video access is granted via presigned URLs generated for AWS S3
5. Monitoring systems collect metrics and trigger alerts if needed

---
