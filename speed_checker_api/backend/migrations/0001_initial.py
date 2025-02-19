# Generated by Django 5.0.6 on 2024-12-13 16:37

import django.utils.timezone
from django.db import migrations, models


class Migration(migrations.Migration):

    initial = True

    dependencies = [
    ]

    operations = [
        migrations.CreateModel(
            name='Radar',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('tempo', models.FloatField()),
                ('distancia', models.FloatField()),
                ('velocidade', models.FloatField()),
                ('limite', models.BooleanField(default=False)),
                ('timestamp', models.DateField(default=django.utils.timezone.now)),
            ],
        ),
    ]
