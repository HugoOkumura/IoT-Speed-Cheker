from django.db import models

from django.utils import timezone
# Create your models here.


class Radar(models.Model):
    speed = models.FloatField()
    timestamp = models.DateField(default=timezone.now)
    limit = models.BooleanField(default=False)

