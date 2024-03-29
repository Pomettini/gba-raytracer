#include "raytracer.c"

// Thanks to purplealienplanet.com

int main()
{
	// Cartridge init
	*(unsigned int *)0x04000000 = 0x0403;

	ray r;

	material materials[3];

	materials[0].diffuse.red = 1;
	materials[0].diffuse.green = 0;
	materials[0].diffuse.blue = 0;

	materials[1].diffuse.red = 0;
	materials[1].diffuse.green = 1;
	materials[1].diffuse.blue = 0;

	materials[2].diffuse.red = 0;
	materials[2].diffuse.green = 0;
	materials[2].diffuse.blue = 1;

	sphere spheres[3];

	spheres[0].pos.x = 60;
	spheres[0].pos.y = 60;
	spheres[0].pos.z = 0;
	spheres[0].radius = 60;
	spheres[0].material = materials[0];

	spheres[1].pos.x = 120;
	spheres[1].pos.y = 80;
	spheres[1].pos.z = 20;
	spheres[1].radius = 50;
	spheres[1].material = materials[1];

	spheres[2].pos.x = 180;
	spheres[2].pos.y = 100;
	spheres[2].pos.z = 40;
	spheres[2].radius = 40;
	spheres[2].material = materials[2];

	light lights[1];

	lights[0].pos.x = 0;
	lights[0].pos.y = 0;
	lights[0].pos.z = -600;
	lights[0].intensity.red = 1;
	lights[0].intensity.green = 1;
	lights[0].intensity.blue = 1;

	int x, y;
	for (y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (x = 0; x < SCREEN_WIDTH; x++)
		{
			float red = 0;
			float green = 0;
			float blue = 0;

			int level = 0;
			float coef = 1.0f;

			r.start.x = x;
			r.start.y = y;
			r.start.z = -2000;

			r.dir.x = 0;
			r.dir.y = 0;
			r.dir.z = 1;

			do
			{
				float t = 20000.0f;
				int current_sphere = -1;
				int i;

				for (i = 0; i < 3; i++)
				{
					if (intersect_ray_sphere(&r, &spheres[i], &t))
						current_sphere = i;
				}
				if (current_sphere == -1)
					break;

				vector scaled = vector_scale(t, &r.dir);
				vector new_start = vector_add(&r.start, &scaled);

				vector n = vector_sub(&new_start, &spheres[current_sphere].pos);
				float temp = vector_dot(&n, &n);
				if (temp == 0)
					break;

				int j;
				for (j = 0; j < 1; j++)
				{
					light current_light = lights[j];
					vector dist = vector_sub(&current_light.pos, &new_start);
					if (vector_dot(&n, &dist) <= 0.0f)
						continue;
					float t = square_root(vector_dot(&dist, &dist));
					if (t <= 0.0f)
						continue;

					ray light_ray;
					light_ray.start = new_start;
					light_ray.dir = vector_scale((1 / t), &dist);

					float lambert = vector_dot(&light_ray.dir, &n) * coef;
					red += lambert * current_light.intensity.red *
						   spheres[current_sphere].material.diffuse.red;
					green += lambert * current_light.intensity.green *
							 spheres[current_sphere].material.diffuse.green;
					blue += lambert * current_light.intensity.blue *
							spheres[current_sphere].material.diffuse.blue;
				}

				level++;
			} while ((coef > 0.0f) && (level < 1));

			float r = min(red * 255.0f, 255.0f);
			float g = min(green * 255.0f, 255.0f);
			float b = min(blue * 255.0f, 255.0f);

			put_pixel(x, y, rgb(r, g, b));
		}
	}

	while (1);

	return 0;
}
