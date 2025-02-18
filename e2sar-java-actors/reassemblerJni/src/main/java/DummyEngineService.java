import java.util.Set;

import org.jlab.epsci.ersap.base.ErsapUtil;
import org.jlab.epsci.ersap.engine.Engine;
import org.jlab.epsci.ersap.engine.EngineData;
import org.jlab.epsci.ersap.engine.EngineDataType;

public class DummyEngineService implements Engine{

    @Override
    public EngineData configure(EngineData engineData) {
        return null;
    }

    @Override
    public EngineData execute(EngineData engineData) {
        return engineData;
    }

    @Override
    public EngineData executeGroup(Set<EngineData> set) {
        return null;
    }

    @Override
    public Set<EngineDataType> getInputDataTypes () {
        return ErsapUtil.buildDataTypes(EngineDataType.JSON,EngineDataType.BYTES);
    }

    @Override
    public Set<EngineDataType> getOutputDataTypes () {
        return ErsapUtil.buildDataTypes(EngineDataType.BYTES);
    }

    @Override
    public Set<String> getStates () {
        return null;
    }

    @Override
    public String getDescription () {
        return "This actor just pushes ByteBuffers to sink actors";
    }

    @Override
    public String getVersion () {
        return "v1.0";
    }

    @Override
    public String getAuthor () {
        return "Srinivas Sivakumar";
    }

    @Override
    public void reset () {

    }

    @Override
    public void destroy () {
    }
}