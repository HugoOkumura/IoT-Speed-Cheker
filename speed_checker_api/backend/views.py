from django.shortcuts import render

from rest_framework.response import Response
from rest_framework.views import APIView 
from rest_framework import status

from .models import Radar
from .serializers import RadarSerializer
# Create your views here.


class SpeedLogs(APIView):

    def get(self, resquest):
        data = Radar.objects.all()
        serializer = RadarSerializer(data, many=True)
        return Response(serializer, status=status.HTTP_200_OK)
    
    def post(self, request):
        serializer = RadarSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)