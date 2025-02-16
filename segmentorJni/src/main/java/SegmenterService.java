import org.jlab.epsci.ersap.engine.EngineData;
import org.jlab.epsci.ersap.engine.EngineDataType;
import org.jlab.epsci.ersap.std.services.AbstractEventWriterService;
import org.jlab.epsci.ersap.std.services.EventWriterException;
import org.json.JSONObject;
import org.jlab.hpdf.EjfatURI;
import org.jlab.hpdf.LbManager;
import org.jlab.hpdf.Segmenter;
import org.jlab.hpdf.exceptions.E2sarNativeException;

import java.io.File;  // Import the File class
import java.io.FileNotFoundException;  // Import this class to handle errors
import java.io.FileReader;
import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

import javax.imageio.ImageWriter;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;


import java.io.IOException;
import java.net.InetAddress;
import java.nio.ByteOrder;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;
import java.nio.charset.Charset;
/**
 * A service that sends data to Segmenter. I am just leaving this as BufferedWriter for the moment, Vardan you probably have to update this for your pipeline
 */
public class SegmenterService extends AbstractEventWriterService<BufferedWriter> {
    static{
        System.loadLibrary("jnie2sar");
    }

    Segmenter segmenter;
    int succesCount;
    int errCount;
    public SegmenterService(){
        succesCount = 0;
        errCount = 0;
    }

    @Override
    protected BufferedWriter createWriter(Path file, JSONObject opts) throws EventWriterException {
        try {
            String iniFile = opts.optString("INI_FILE", "segmentor.ini");
            String stringUri = opts.getString("EJFAT_URI");
            String senderIp = opts.optString("SENDER_IP", "127.0.0.1");

            boolean preferV6 = opts.optBoolean("PREFER_V6", false);
            boolean preferHostAddress = opts.optBoolean("PREFER_HOST_ADDRESS", false);
            boolean validate = opts.optBoolean("VALIDATE", false);
            boolean useCp = opts.optBoolean("USE_CP", false);

            int dataId = opts.optInt("DATA_ID", 5555);
            long eventSrcId = opts.optLong("EVENT_SRC_ID", 7777);

            EjfatURI ejfatURI = EjfatURI.createInstance(stringUri, EjfatURI.Token.INSTANCE, preferV6);
            System.out.println("EJFAT_URI= " + stringUri);
            System.out.println("senderIp= " + senderIp);
            System.out.println("validate= " + validate);
            System.out.println("preferHostAddress= " + preferHostAddress);
            if(useCp){
                addSender(ejfatURI, senderIp, validate, preferHostAddress);
            }
            segmenter = new Segmenter(ejfatURI, dataId, eventSrcId, iniFile);
            segmenter.openAndStart();
            return new BufferedWriter(new FileWriter(file.toFile()));//I might just write the input to output or ignore it for performace 
        }
        catch (FileNotFoundException e) {
            throw new EventWriterException("Could not create writer", e);
        }
        catch(IOException e){
            e.printStackTrace();
        }
        catch(E2sarNativeException e){
            e.printStackTrace();
            System.exit(-1);
        }
        return null;
    }

    private void addSender(EjfatURI dpUri, String senderIp, boolean validate, boolean preferHostAddress){
        System.out.println("EJFAT_URI= " + dpUri.toString(EjfatURI.Token.INSTANCE));
        System.out.println("senderIp= " + senderIp);
        System.out.println("validate= " + validate);
        System.out.println("preferHostAddress= " + preferHostAddress);

        List<String> senders = new ArrayList<>();
        senders.add(senderIp);
        System.out.println("Adding senders to LB: ");
        for(String sender : senders){
            System.out.print(sender + " ");
        }
        System.out.println();

        try {
            LbManager lbman = new LbManager(dpUri, validate, preferHostAddress);
            lbman.addSenders(senders);
            System.out.println("Added Senders successfully");
            lbman.free();
        } catch (E2sarNativeException e) {
            System.out.println("Unable to add a sender due to error");
            e.printStackTrace();
            System.exit(-1);
        }
    }

    @Override
    protected void closeWriter() {
        try {
            System.out.println("Number of events sent = " + succesCount);
            segmenter.free();
            writer.close();
        } catch (Exception e) {
            // ignore
        }
    }

    @Override
    protected void writeEvent(Object event) throws EventWriterException {
        ByteBuffer buffer = (ByteBuffer) event;
        try {
            segmenter.addToSendQueueDirect(buffer, 0, 0, 0);
        } catch (Exception e) {
            e.printStackTrace();
            errCount++;
            return;
        }
        succesCount++;
    }

    @Override
    protected EngineDataType getDataType() {
        return EngineDataType.BYTES;
    }
}