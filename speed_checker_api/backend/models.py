from django.db import models

from django.utils import timezone
# Create your models here.


class Radar(models.Model):
    tempo = models.IntegerField()
    distancia = models.IntegerField()
    velocidade = models.IntegerField()
    limite = models.BooleanField(default=False)
    timestamp = models.DateField(default=timezone.now)

