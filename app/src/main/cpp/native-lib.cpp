#include <jni.h>
#include <string>

extern "C" JNIEXPORT jfloat JNICALL
Java_com_iauto_juiceohmeter_MainActivity_getBatteryTemperatureFromJNI(
        JNIEnv* env,
        jobject /* this */,
        jobject batteryStatus) {
    // Get the BatteryManager class
    jclass batteryManagerClass = env->FindClass("android/os/BatteryManager");

    // Get the EXTRA_TEMPERATURE field ID
    jfieldID extraTemperatureFieldID = env->GetStaticFieldID(batteryManagerClass,
                                                             "EXTRA_TEMPERATURE",
                                                             "Ljava/lang/String;");

    // Get the EXTRA_TEMPERATURE string
    jstring extraTemperatureString = (jstring) env->GetStaticObjectField(batteryManagerClass,
                                                                         extraTemperatureFieldID);

    // Get the temperature from the Intent
    jclass intentClass = env->GetObjectClass(batteryStatus);
    jmethodID getIntExtraMethod = env->GetMethodID(intentClass, "getIntExtra",
                                                   "(Ljava/lang/String;I)I");
    jint temperature = env->CallIntMethod(batteryStatus, getIntExtraMethod, extraTemperatureString,
                                          -1);

    // Convert temperature to Celsius
    return temperature / 10.0f;
}

// Add new method for voltage
extern "C" JNIEXPORT jint JNICALL
Java_com_iauto_juiceohmeter_MainActivity_getBatteryVoltageFromJNI(
    JNIEnv* env,
    jobject /* this */,
    jobject batteryStatus) {

    jclass intentClass = env->GetObjectClass(batteryStatus);
    jmethodID getIntExtra = env->GetMethodID(intentClass, "getIntExtra", "(Ljava/lang/String;I)I");
    jstring extraVoltage = env->NewStringUTF("voltage");

    return env->CallIntMethod(batteryStatus, getIntExtra, extraVoltage, 0);
}


extern "C" JNIEXPORT jint JNICALL
Java_com_iauto_juiceohmeter_MainActivity_getBatteryCurrentFromJNI(
        JNIEnv* env,
        jobject /* this */,
        jobject batteryManager) {

    jclass batteryManagerClass = env->GetObjectClass(batteryManager);
    jmethodID getIntProperty = env->GetMethodID(batteryManagerClass, "getIntProperty", "(I)I");

    return env->CallIntMethod(batteryManager, getIntProperty, 2); // BatteryManager.BATTERY_PROPERTY_CURRENT_NOW
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_iauto_juiceohmeter_MainActivity_getPlugginInFromJNI(
        JNIEnv* env,
        jobject /* this */,
        jobject batteryManager) {

    jclass batteryManagerClass = env->GetObjectClass(batteryManager);
    jmethodID getIntProperty = env->GetMethodID(batteryManagerClass, "getIntProperty", "(I)I");

    jint status = env->CallIntMethod(batteryManager, getIntProperty, 6); // BatteryManager.BATTERY_PROPERTY_STATUS

    // Check if battery is charging (status == 2) or full (status == 5)
    if (status == 2 || status == 5) {
        return 1; // Plugged in
    } else {
        return 0; // Not plugged in
    }
}