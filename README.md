# ersap-e2sar

This project creates Java and CPP E2SAR actors for [ERSAP](https://github.com/JeffersonLab/ersap-java/tree/main). Pipelines are implemented in the subdirectories [e2sar-actors](e2sar-actors) and [e2sar-java-actors](e2sar-java-actors/). Both of these subdirectories contain a segmenter and reassembler pipeline. The difference between these two are as follows

1. [e2sar-actors](e2sar-actors) - These are pipelines where the [E2SAR](https://github.com/JeffersonLab/E2SAR) actors are written in C++ and source/sink actors are written in Java. 
2. [e2sar-java-actors](e2sar-java-actors/) - These are pipelines where [E2SAR-JAVA](https://github.com/JeffersonLab/e2sar-java) is used for segmentation and reassembly. This is a pure java pipeline where E2SAR-JAVA uses JNI to link with E2SAR native code. 

The requirements and build process for the pipelines do differ so they are described in the subdirectories. 

There also [Jupyter notebooks](scripts/) which run on FABRIC that show examples of using and running both sets of pipelines

## Prerequisites

* Set ERSAP_HOME environment variable which will be used to install the ERSAP environment and the actors in this project
* Download and install [ersap-java](https://github.com/JeffersonLab/ersap-java/tree/main) [ersap-cpp](https://github.com/JeffersonLab/ersap-cpp/tree/main) and the corresponding dependencies of the respective projects
* Download and install e2sar debian package from [e2sar-releases](https://github.com/JeffersonLab/E2SAR/releases). This contains all necessary dependencies for E2SAR

## Description of actors

There are two subprojects `segmentor` and `reassembler` which have the following actors.

* Segmentor
    * FileReaderService(Java source actor) - This actor simply reads data from an input file specfied by set fileList in the ERSAP shell. It outputs events as a ByteBuffer for processing.
    * ByteBufferInputService(Java source actor) - This actor generates arbitrary ByteBuffers which can be configured as a parameter in the services.yaml file. See [byte_services.yaml](segmentor/config/live/byte_services.yaml)
    * DummyWriterService(Java sink actor) - This is a dummy actor which does nothing. Needed because ERSAP does not support CPP source and sink actors yet
    * segmentor_actor(CPP E2SAR actor) - This builds the Segmentor object which will segment the events and send sync/data messages to the load balancer. The Segmentor also registers itself with the LB during configuration. The configuration for the segmentor is sepcified in INI files and the path needs to be specified in the services.yaml. See [segmentor.ini](segmentor/config/live/segmentor.ini) and [services.yaml](segmentor/config/live/services.yaml)

* Reassembler
    * BufferSizeReaderService(Java source actor) - This actor simply gets the number of events from the services.yaml file. It outputs 0 as the first event and all remaining events are 1. 0 is used as a prestart condition for setting up reassembler wait times.
    * FileWriterService(Java sink actor) - This actor writes events to a file.
    * reassembler_actor(CPP E2SAR actor) - This builds the reassembler object which will reassemble the events and send sync messages to the control plane. In the configure(), this actor will register itself with the loadbalancer. The configuration for the reassembler is sepcified in INI files and the path needs to be specified in the services.yaml. See [reassembler.ini](reassembler/config/live/segmentor.ini) and [services.yaml](reassembler/config/live/services.yaml)

## Fabric examples

In the scripts tab there are two notebooks with examples on how to set up and run this project. 