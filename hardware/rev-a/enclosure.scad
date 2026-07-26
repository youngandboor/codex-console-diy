// Codex Console Alpha Rev A mechanical reference.
// Units are millimeters. Print prototypes only after checking the real parts.
$fn = 64;

outer_w = 154;
outer_d = 116;
body_h = 18;
wall = 2.0;
corner_r = 6;
plate_t = 1.5;
pcb_z = 11;

key_centers = [
    [18, 20], [37.05, 20], [56.10, 20],
    [18, 39.05], [37.05, 39.05], [56.10, 39.05],
    [18, 65], [37.05, 65], [56.10, 65], [75.15, 65],
    [27.525, 89], [56.10, 89]
];

mounts = [[6, 6], [148, 6], [6, 110], [148, 110], [95, 110]];

module rounded_rect_2d(w, d, r) {
    offset(r = r) square([w - 2*r, d - 2*r], center = true);
}

module rounded_box(w, d, h, r) {
    linear_extrude(height = h) rounded_rect_2d(w, d, r);
}

module at_xy(p, z = 0) {
    translate([p[0] - outer_w/2, p[1] - outer_d/2, z]) children();
}

module bottom_shell() {
    difference() {
        rounded_box(outer_w, outer_d, body_h, corner_r);
        translate([0, 0, 2]) rounded_box(outer_w - 2*wall, outer_d - 2*wall,
                                         body_h, corner_r - wall);
        // Rear connector openings: USB-C and power slide switch.
        translate([38 - outer_w/2, -outer_d/2, 9]) cube([17, 8, 8], center = true);
        translate([136 - outer_w/2, -outer_d/2, 9]) cube([15, 8, 6], center = true);
    }

    for (p = mounts) {
        at_xy(p, 2) difference() {
            cylinder(h = pcb_z - 2, d = 7);
            cylinder(h = pcb_z, d = 2.7);
        }
    }
}

module switch_plate() {
    difference() {
        linear_extrude(height = plate_t)
            rounded_rect_2d(150, 112, 4);

        for (p = key_centers) {
            at_xy(p, plate_t/2) cube([14.0, 14.0, plate_t + 0.2], center = true);
        }

        // EC11 shaft, joystick travel, and communication touch insert.
        at_xy([119, 22], -0.1) cylinder(h = plate_t + 0.2, d = 7.2);
        at_xy([119, 60], -0.1) cylinder(h = plate_t + 0.2, d = 22);
        at_xy([119, 94], plate_t/2) cube([22, 16, plate_t + 0.2], center = true);

        // Approximate 2U stabilizer access. Replace with the selected stabilizer drawing.
        at_xy([27.525 - 11.9, 89], plate_t/2)
            cube([6.8, 12, plate_t + 0.2], center = true);
        at_xy([27.525 + 11.9, 89], plate_t/2)
            cube([6.8, 12, plate_t + 0.2], center = true);

        for (p = mounts) {
            at_xy(p, -0.1) cylinder(h = plate_t + 0.2, d = 3.2);
        }
    }
}

module battery_dummy() {
    color([0.3, 0.55, 0.8, 0.45])
        at_xy([45, 88], 5.7) cube([70, 40, 7], center = true);
}

module pcb_dummy() {
    color([0.1, 0.45, 0.25, 0.45])
        translate([0, 0, pcb_z]) rounded_box(148, 110, 1.6, 2);
}

// Assembly view. Comment individual modules to export separate parts.
color([0.20, 0.22, 0.24]) bottom_shell();
battery_dummy();
pcb_dummy();
color([0.08, 0.09, 0.10]) translate([0, 0, pcb_z + 6.6]) switch_plate();
