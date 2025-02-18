# e2sar-java-actors

This project contains two pipelines, segmenterJni and reassemblerJni. These actors are written in Java and use DirectByteBuffers to to move data. 

## Prerequisites

* Java 17 and gradle
* Set ERSAP_HOME environment variable which will be used to install the ERSAP environment and the actors in this project `export ERSA_HOME=/path/to/ersap-install`
* Download and install [ersap-java](https://github.com/JeffersonLab/ersap-java/tree/main) and the dependencies related
* Download and install the E2SAR debian/rpm package from [e2sar-releases](https://github.com/JeffersonLab/E2SAR/releases). This contains all necessary dependencies to run E2SAR. 
* Download, build and install the [E2SAR-JAVA](https://github.com/JeffersonLab/e2sar-java) which links with the E2SAR deb/rpm. Follow the instructions on the repo to install the jar.

## Description of actors

There are two pipelines `segmentorJni` and `reassemblerJni` which have the following actors.

* SegmentorJni
    * `FileReaderService`(Java source actor) - This actor simply reads data from an input file specfied by set fileList in the ERSAP shell. It outputs events as a Direct ByteBuffer for processing.
    * `ByteBufferInputService`(Java source actor) - This actor generates arbitrary Direct ByteBuffers which can be configured as a parameter in the services.yaml file. See [byte_services.yaml](segmentorJni/config/live/byte_services.yaml)
    * `DummyEngineService`(Java engine actor) - This actor just takes in a ByteBuffer and pushes it to the Sink actor. (One Engine actor is needed in the pipeline)
    * `SegmenterService`(Java sink actor) - This builds the Segmenter object which will segment the events and send sync/data messages to the load balancer. The Segmentor also registers itself with the LB during configuration (if Control plane is used). The configuration for the segmentor is sepcified in INI files and the path needs to be specified in the services.yaml. See [segmentor.ini](segmentorJni/config/live/segmentor.ini) and [byte_services.yaml](segmentorJni/config/live/byte_services.yaml)

* ReassemblerJni
    * `ReassemblerService`(Java source actor) -  This builds the reassembler object which will reassemble the events and send sync messages to the control plane. In the configure(), this actor will register itself with the loadbalancer (if Control plane is used). The configuration for the reassembler is sepcified in INI files and the path needs to be specified in the services.yaml. See [reassembler.ini](reassemblerJni/config/live/reassembler.ini) and [services.yaml](reassemblerJni/config/live/services.yaml)
    * `FileWriterService`(Java sink actor) - This actor writes events to a file.
    * `DummyEngineService`(Java engine actor) - This actor just takes in a ByteBuffer and pushes it to the Sink actor. (One Engine actor is needed in the pipeline)

## Building and installing actors

`ERSAP_HOME` needs to be set to install the actors in the right directory. The build process uses gradle to build and install the JAR to `$ERSAP_HOME/lib`

Use `./gradlew install` in the segmentorJni and reassemblerJni folders to install the actors.

## Configuration

There are two ways we can run this pipeline, with and without a load balancer. The config for running the pipelines is located in `config/localhost` and `config/live`.

The config for the pipeline is specified in the `*service.yaml` file.
The config for segmentation/reassembly is specified in `ini` files

* Segmentor - [byte_service.yaml](segmentorJni/config/live/byte_services.yaml)
    * `INI_FILE` : This is the path to the ini file that the E2sar Segmenter will use.
    * `EJFAT_URI` : Needs to be updated with Instance URI for live test
    * `PREFER_V6` : Prefer ipv6 over ipv4 when talking to CP
    * `PREFER_HOST_ADDRESS` : true/false (if true, will use ip address instead of hostname while connecting to LB CP)
    * `SENDER_IP` : IP of the node running the segmenter
    * `VALIDATE` : true/false (Depending on whether you want to enable certificate validation)
    * `DATA_ID` : Data ID of experiment
    * `EVENT_SRC_ID` : Event Source ID of experiment
    * `USE_CP` : true for live tests and false for localhost test
* Reassembler - [services.yaml](reassemblerJni/config/live/services.yaml)
    * `INI_FILE` : This is the path to the ini file that the E2sar Segmenter will use.
    * `EJFAT_URI` : Needs to be updated with Instance URI for live test
    * `PREFER_V6` : Prefer ipv6 over ipv4 when talking to CP
    * `WORKER_IP` : IP of the node running the reassembler
    * `WORKER_PORT` : Starting UDP port number the reassembler will be listening on
    * `HOSTNAME` : Any unique hostname of the node running the reassembler
    * `TIMEOUT` : The timeout for checking if events are present in reassembly event queue
    * `REASSEMBLER_THREADS` :Number of reassembler threads
    * `USE_CP` : true for live tests, false for localhost tests


#### Some additional configuration information

The `USE_CP` flag is duplicated from the ini files. This is needed so that the Java code know if it should contact the LB to register senders/workers. Keep this flag consistent with the INI file.

Since this uses E2SAR-JAVA which has a JNI shared object, our java actors must be able to link with it. You need to set `-Djava.library.path=/path/to/install/directory` in the ERSAP shell using `set javaOptions` or set it in the `.ersap` files in config


## Running the pipelines

### Running localhost pipeline (without LB)

There is a jupyter [localhost notebook](../scripts/E2SAR-JAVA-ERSAP-localhost.ipynb) which runs on FABRIC that can be used to run an entire pipleine detailing the process of building and running this pipeline for reference.  

The localhost pipeline is meant to test if everything is running as expected without a LB. This pipeline is designed to be run on the same host and generally the configuration need not be updated. 

The segmentor pipeline uses the `FileReaderService` source actor that reads in the file specified in [in.txt](segmentorJni/input/in.txt), which expects the number of events to be first line of input and subsequent lines being any string.

The reassembler pipeline uses the `ReassemblerService` source actor that indefinitely waits for events. You need to stop the pipeline using `SIGINT`.

To run the localhost pipeline, start the reassembler pipeline first so that is ready to receive data using the following command
```bash
$ $ERSAP_HOME/bin/ersap-shell reassemblerJni/config/localhost/reassembler_localhost.ersap
```

Then you can run the segmentor pipeline in another shell using
```bash
$ $ERSAP_HOME/bin/ersap-shell segmentorJni/config/localhost/segmentor_localhost.ersap
```

Once the pipelines complete, you can check `reassemblerJni/output/out_in.txt` to see if all the events have been received and reassembled. 


### Running live pipeline (with LB)

There is a jupyter [live notebook](../scripts/E2SAR-JAVA-ERSAP-fabric-lb-tester.ipynb) which runs on FABRIC that works with a LB deployed in FABRIC. It can be used to run an entire pipleine detailing the process of building and running for reference.

A new/existing LB reservation must be created and the yaml files must be updated with the `EJFAT_URI`.

The segmentor uses `ByteBufferInputService` to create ByteBuffers as input which can be configured in [byte_services.yaml](segnmentorJni/config/live/byte_services.yaml). `eventSize` and `events` can be used to configure the size of each event and the number of events that will be generated respectively.

The reassembler uses the same actors as the localhost test. You need to stop the pipeline using `SIGINT`.

Similar to the localhost test, run the `reassembler` pipeline first so that it is ready to accept events, followed by the `segmenter`

To run the live reassembler pipeline
```bash
$ $ERSAP_HOME/bin/ersap-shell reassemblerJni/config/live/reassembler_live.ersap
```

To run the live segmentor 
```bash
$ $ERSAP_HOME/bin/ersap-shell segmentorJni/config/live/segmentor_live.ersap
```

Once the pipelines complete, you can check `reassembler/output/out_in.txt` to see if all the events have been received and reassembled. 
