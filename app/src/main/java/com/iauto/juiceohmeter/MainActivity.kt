package com.iauto.juiceohmeter

import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.os.BatteryManager
import android.os.Bundle
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import com.iauto.juiceohmeter.databinding.ActivityMainBinding
import kotlinx.coroutines.*

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private lateinit var textView: TextView
    private var updateJob: Job? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        textView = binding.sampleText

        // Start updating battery info
        startUpdatingBatteryInfo()
    }

    private fun startUpdatingBatteryInfo() {
        updateJob = CoroutineScope(Dispatchers.Main).launch {
            while (isActive) {
                updateBatteryInfo()
                delay(1000) // Wait for 1 second
            }
        }
    }

    private fun updateBatteryInfo() {
        val intentFilter = IntentFilter(Intent.ACTION_BATTERY_CHANGED)
        val batteryStatus = registerReceiver(null, intentFilter)
        val batteryManager = getSystemService(Context.BATTERY_SERVICE) as BatteryManager

        if (batteryStatus != null) {
            val nativeTemperature = getBatteryTemperatureFromJNI(batteryStatus)
            val nativeVoltage = getBatteryVoltageFromJNI(batteryStatus)
            val nativeCurrent = getBatteryCurrentFromJNI(batteryManager)
            val pluggedIn = getPlugginInFromJNI(batteryManager)
            val nativeWattage = nativeVoltage * nativeCurrent / 1000

            // Display temperature, voltage, and current
            textView.text = String.format(
                "Battery Temperature: %.1f°C\n" +
                        "Battery Voltage: %d mV\n" +
                        "Battery Current: %d mA\n" +
                        "Battery Power: %d mW\n" +
                        "Plugged In: %b",
                nativeTemperature,
                nativeVoltage,
                nativeCurrent,
                nativeWattage,
                pluggedIn
            )
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        // Cancel the coroutine job when the activity is destroyed
        updateJob?.cancel()
    }

    // Native method declarations
    external fun getBatteryTemperatureFromJNI(batteryStatus: Intent): Float
    external fun getBatteryVoltageFromJNI(batteryStatus: Intent): Int
    external fun getBatteryCurrentFromJNI(batteryManager: BatteryManager): Int
    external fun getPlugginInFromJNI(batteryManager: BatteryManager): Boolean

    companion object {
        // Used to load the 'juiceohmeter' library on application startup.
        init {
            System.loadLibrary("juiceohmeter")
        }
    }
}