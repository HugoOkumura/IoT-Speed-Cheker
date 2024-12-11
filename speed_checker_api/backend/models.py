from django.db import models

from django.utils import timezone
# Create your models here.


class Radar(models.Model):
    tempo = models.FloatField()
    distancia = models.FloatField()
    velocidade = models.FloatField()
    limite = models.BooleanField(default=False)
    timestamp = models.DateField(default=timezone.now)

