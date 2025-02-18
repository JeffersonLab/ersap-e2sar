# e2sar-actors

This project contains two pipelines, segmenter and reassembler. This pipeline also cannot use Direct ByteBuffers as there is serialization between Java and C++ actors.

## Prerequisites

* CMake and C++ 17
* Java 17 and gradle
* Set ERSAP_HOME environment variable which will be used to install the ERSAP environment and the actors in this project `export ERSA_HOME=/path/to/ersap-install`
* Download and install [ersap-java](https://github.com/JeffersonLab/ersap-java/tree/main) [ersap-cpp](https://github.com/JeffersonLab/ersap-cpp/tree/main) and the corresponding dependencies of the respective projects
* Download and install the E2SAR debian/rpm package from [e2sar-releases](https://github.com/JeffersonLab/E2SAR/releases). This contains all necessary dependencies to run E2SAR. 

## Description of actors

There are two pipelines `segmentor` and `reassembler` which have the following actors.

* Segmentor
    * JAVA Actors:
        * `FileReaderService`(Java source actor) - This actor simply reads data from an input file specfied by set fileList in the ERSAP shell. It outputs events as a ByteBuffer for processing.
        * `ByteBufferInputService`(Java source actor) - This actor generates arbitrary ByteBuffers which can be configured as a parameter in the services.yaml file. See [byte_services.yaml](segmentor/config/live/byte_services.yaml)
        * `DummyWriterService`(Java sink actor) - This is a dummy actor which does nothing. Needed because ERSAP does not support CPP source and sink actors yet
    * C++ Actors:
        * `segmentor_actor`(CPP E2SAR actor) - This builds the Segmentor object which will segment the events and send sync/data messages to the load balancer. The Segmentor also registers itself with the LB during configuration (if Control plane is used). The configuration for the segmentor is sepcified in INI files and the path needs to be specified in the services.yaml. See [segmentor.ini](segmentor/config/live/segmentor.ini) and [services.yaml](segmentor/config/live/services.yaml)

* Reassembler
    * JAVA Actors:
        * `BufferSizeReaderService`(Java source actor) - This actor simply gets the number of events from the services.yaml file. It outputs 0 as the first event and all remaining events are 1. 0 is used as a prestart condition for setting up reassembler wait times.
        * FileWriterService(Java sink actor) - This actor writes events to a file.
    * C++ Actors:
        * `reassembler_actor`(CPP E2SAR actor) - This builds the reassembler object which will reassemble the events and send sync messages to the control plane. In the configure(), this actor will register itself with the loadbalancer (if Control plane is used). The configuration for the reassembler is sepcified in INI files and the path needs to be specified in the services.yaml. See [reassembler.ini](reassembler/config/live/segmentor.ini) and [services.yaml](reassembler/config/live/services.yaml)

## Building and installing actors

`ERSAP_HOME` needs to be set to install the actors in the right directory. The build process for the actors is as below:

1. JAVA actors:
    * Uses gradle to build and install the JAR to `$ERSAP_HOME/lib`
    * Run the following command in both segmentor and reassembler to compile and install the JAR `./gradlew install`
2. CPP actors:
    * `$PKG_CONFIG_PATH` and `LD_LIBRARY_PATH` needs to be set to point to the E2SAR install location.
    * Run the following command in both segmentor and reassembler to compile and install the shared object library 
    ```bash
    $ cmake -S . -B build
    $ cmake --build build --target install 
    ```

## Running the pipelines

There are two ways we can run this pipeline, with and without a load balancer. The config for running the pipelines is located in `config/localhost` and `config/live`. A design decision here is that any parameters needed by the C++ actors are stored in `ini` files and the parameters needed by Java actores are stored in the `*services.yaml` file used by ERSAP.

### Running localhost pipeline (without LB)

There is a jupyter [localhost notebook](../scripts/E2SAR-ERSAP-localhost.ipynb) which runs on FABRIC that can be used to run an entire pipleine detailing the process of building and running this pipeline for reference.  

The localhost pipeline is meant to test if everything is running as expected without a LB. This pipeline is designed to be run on the same host and generally the configuration need not be updated. 

The segmentor pipeline uses the `FileReaderService` source actor that reads in the file specified in [in.txt](segmentor/input/in.txt), which expects the number of events to be first line of input and subsequent lines being any string.

The reassembler pipeline uses the `BufferSizeReaderService` source actor that gets the parameter on number of events expected from [services.yaml](reassembler/config/localhost/services.yaml), for the expected number of events. 

To run the localhost pipeline, start the reassembler pipeline first so that is ready to receive data using the following command
```bash
$ $ERSAP_HOME/bin/ersap-shell reassembler/config/localhost/reassembler_localhost.ersap
```

Then you can run the segmentor pipeline in another shell using
```bash
$ $ERSAP_HOME/bin/ersap-shell segmentor/config/localhost/segmentor_localhost.ersap
```

Once the pipelines complete, you can check `reassembler/output/out_in.txt` to see if all the events have been received and reassembled. 

#### Some additional configuration information

Since this pipeline does not use the LB, the ini files contain a flag `useCP = false` as seen in [segmentor.ini](segmentor/config/localhost/segmentor.ini) and [reassembler.ini](reassembler/config/localhost/reassembler.ini). This needs to be set to false to avoid trying to contact the LB. 

### Running live pipeline (with LB)

There is a jupyter [live notebook](../scripts/E2SAR-ERSAP-live-lb.ipynb) which runs on FABRIC that works with a live ESNET LB. It can be used to run an entire pipleine detailing the process of building and running for reference. 

#### Config

The config for this test needs to be updated before it can be run. In the [segmentor.ini](segmentor/config/live/segmentor.ini) and [reassembler.ini](reassembler/config/live/reassembler.ini), there is a category called `[lb-config]` that needs to be updated. 

A new/existing LB reservation must be created and the INI files must be updated with the `ejfatUri`.
`useCP = true` is also needed to talk to the LB.

* [segmentor.ini](segmentor/config/live/segmentor.ini) - `[lb-config]`
    * ejfatUri = Needs to be updated with Instance URI
    * senderIP = IP of the node running the segementor
    * validate = true/false (Depending on whether you want to enable certificate validation)
    * preferHostAddress = true/false (if true, will use ip address instead of hostname while connecting to LB CP)
* [reassembler.ini](reassembler/config/live/reassembler.ini) - `[lb-config]`
    * ejfatUri = Needs to be updated with Instance URI
    * ip = IP of the node running reassembler
    * port = Starting UDP port number the reassembler will be listening on
    * initTimeout = Initial timeout for events (On startup it might take longer to receive events)
    * timeout = Timeout for events (Once first event is received, the timeout will be switched to this value)
    * threads = Number of reassembler threads

The segmentor uses `ByteBufferInputService` to create ByteBuffers as input which can be configured in [byte_services.yaml](segmentor/config/live/byte_services.yaml). `eventSize` and `events` can be used to configure the size of each event and the number of events that will be generated respectively.

The reassembler uses the same actors as the localhost test but we still need to provide the number of events in [services.yaml](reassembler/config/live/services.yaml). This is a workaround which is explained at the end of the document.

Similar to the localhost test, run the `reassembler` pipeline first so that it is ready to accept events, followed by the `segmenter`

To run the live reassembler pipeline
```bash
$ $ERSAP_HOME/bin/ersap-shell reassembler/config/live/reassembler_live.ersap
```

To run the live segmentor 
```bash
$ $ERSAP_HOME/bin/ersap-shell segmentor/config/live/segmentor_live.ersap
```

Once the pipelines complete, you can check `reassembler/output/out_in.txt` to see if all the events have been received and reassembled. 

## Limitations and Workarounds

Since `ERSAP` only supports Java based source and sink actors there are some limitations. The segmentor is supposed to be a sink actor as it should just send data to the LB and the Reassembler should be the source actor as it just receives data from the LB. 

Another problem is that on the reassembler side it is uncertain how many events are received, if you have multiple worker nodes running. One solution is to keep the number of events as INT_MAX so that you can constantly receive data and then manually stop it using `SIGINT`.

The biggest drawback is having to copy data Java to CPP and vice versa. In HPC environments this might not be the best approach. 

The `E2SAR-JAVA` pipeline should overcome these limitaions having a pure Java pipeline and using Direct ByteBuffers to reduce copying between Java and the native code.
