package InuCommon;

import java.util.HashMap;
import java.util.Map;

public enum EHTSource {
    eVideo(0),
    eWebcam(1);
    
	EHTSource(int code)
	{
		this.mCode = code;
	}

	private static final Map<Integer, EHTSource> typesByValue = new HashMap<Integer, EHTSource>();

    static {
        for (EHTSource type : EHTSource.values()) {
            typesByValue.put(type.mCode, type);
        }
    }

    public static EHTSource forValue(int value) {
        return typesByValue.get(value);
    }

	private int mCode;	
}
