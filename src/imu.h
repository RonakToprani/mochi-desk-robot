#ifndef IMU_H
#define IMU_H

// =============================================================================
// IMU — QMI8658 6-axis accelerometer + gyroscope
// =============================================================================

void imu_init();
void imu_update();

bool imu_is_picked_up();
bool imu_is_shaking();
bool imu_is_face_down();
bool imu_is_tilted();

#endif // IMU_H
