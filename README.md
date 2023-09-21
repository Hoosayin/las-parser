# LAS 2.0 Parser

**las-parser** is an MFC SDI application designed to parse LAS 2.0 file formats. The main functionality of this application is to enable users to select a Curve Attribute from a combobox. Once a curve is selected, it is then drawn as a polyline, using the curve points parsed from the LAS file corresponding to that attribute.

![LAS Parser Screenshot](https://raw.githubusercontent.com/Hoosayin/las-parser/master/screenshot.png)

## What is LAS?

**LAS**, or Log ASCII Standard, is a file format commonly used in the oil and gas industry to store well log data. It defines a standardized way of representing well log data in a plain text format. LAS files contain information about various aspects of the wellbore, including depth, curve data, and more.

## Technical Details

The **Las** class within the application represents the structure of the LAS 2.0 file format. The **LasParser** class is responsible for parsing LAS 2.0 files and creating a Las object for further manipulation. Users can open any LAS 2.0 file from the Menu Bar. If the file is invalid, the first encountered error is displayed on the Status Bar. However, if the file is successfully parsed, the file name is displayed on the Status Bar.

The Combo Box in the application is populated with available Curve Attributes. Curve data is scaled according to the drawing viewport and drawn via the Graphic Device Interface (GDI) as a polyline. The Legend area displays curve values, which are updated when the mouse pointer is moved along the curve.

## Environment

- **IDE:** Visual Studio 2022
- **PROGRAMMING LANGUAGE:** C++
- **TOOLSET:** v142
