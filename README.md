# 3D Interactive Visualization of Vibration Simulation


This is a GitHub project of my master thesis of the same name for the tool I developed using C++ and OpenGL. Additionaly, I used [GLFW](https://github.com/glfw/glfw) for creating application windows, [Dear ImGui](https://github.com/ocornut/imgui) for drawing UI, [ImPlot](https://github.com/epezent/implot) for drawing plots, [glm](https://github.com/g-truc/glm) for math calculations and [NFD](https://github.com/mlabbe/nativefiledialog) for opening native file dialogues.

## Abstract

Minimizing engine noise presents an important task when designing an engine, so it is of utmost importance to keep the engine vibration strength at the lowest possible
level since it directly affects the noise level produced by the engine. NVH (Noise, Vibration, Harshness) simulations provide an easy way to check the vibration strength
of individual engine parts at the design stage. However, computational simulations of this kind often produce data that is hard to analyze, so visualization tools help significantly by presenting the calculated data intuitively and understandably. This thesis describes the development of an application for visualizing data obtained from NVH simulation using a 3D engine model and 2D graphs. The focus of the presented tool is to enable a simpler way of comparing the vibration of engine parts with respect to selected frequencies.

<br />

![general_screenshot](https://user-images.githubusercontent.com/57198780/186611503-f046f65c-142a-4c5d-99d1-5450edbd3f5e.png "General screenshot of the tool")

<br />

## Showcase

The tool supports 2 visualization modes: 

General Mode - engine cells are colored based on various user preferences so that user can have finer control over highlighted details.

<br />

<img src="https://user-images.githubusercontent.com/57198780/186648725-3e54dbdf-19ca-491c-980c-302cd8e9d580.png" width=75%>

<br />

Limits mode - engine cells are colored based on selected harmonics and predefined limits in order to point out loudest parts of the engine at selected harmonics.

<br />

<img src="https://user-images.githubusercontent.com/57198780/186649632-f6fe886a-7a84-4e11-9b37-852859a9a9b7.png" width=75%>

<br />

Engine cells' colors are determined based on selected gradient. User can select end values of gradient and gradient sampling function. Visual results of various sampling functions can be seen in figures below.

<br />

![linear_colormap](https://user-images.githubusercontent.com/57198780/186656053-55f84ecf-7ece-4d66-ba4e-d6264377c6ca.png "Linear sampling") ![cubic_colormap](https://user-images.githubusercontent.com/57198780/186656045-ebbf8b04-ff45-47b3-9bae-b25defdfce60.png "Cubic sampling") ![cubic_symmetrical_colormap](https://user-images.githubusercontent.com/57198780/186656052-09c53d3f-dd5d-401a-8a4c-c1889146383a.png "Cubic symmetrical sampling")  ![quartic_colormap](https://user-images.githubusercontent.com/57198780/186656054-fc8c2eb3-bbb6-4e36-9b1e-02058e315992.png "Quartic sampling") ![quartic_symmetrical_colormap](https://user-images.githubusercontent.com/57198780/186656056-303ae5bb-53f1-40f9-b53c-744251f34418.png "Quartic symmetrical sampling")

<br />

For detailed comparision and analysis of specific cells values, user can utilize various graph modes implemented in the tool. To place a specific cell on the graph, user must select it by left-clicking it in the engine view window. After that, cell will be colored with it's graph color. Cell can be deselected by left-clicking.

![normal_bars](https://user-images.githubusercontent.com/57198780/186657438-ed488ded-d27f-451c-ad70-683157697eef.png) ![relative_bars](https://user-images.githubusercontent.com/57198780/186657443-ae229072-9c9d-4fda-8e67-5482418c21d3.png) ![subplots_bars](https://user-images.githubusercontent.com/57198780/186657446-0f9b586a-7569-40b9-b692-9399045ea1d9.png)

For more in-depth analysis of the tool check out the [full master thesis](https://github.com/wolfdeleeya/3D-Interactive-Visualization-of-Vibration-Simulation/blob/thesis_paper_and_presentation/thesis.pdf).
