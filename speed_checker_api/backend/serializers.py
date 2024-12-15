from rest_framework import serializers
from .models import Radar


class RadarSerializer(serializers.ModelSerializer):
    class Meta:
        model = Radar
        fields = ['tempo', "distancia", "velocidade", "limite"]

