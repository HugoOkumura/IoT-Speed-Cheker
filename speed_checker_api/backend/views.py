from django.shortcuts import render

from rest_framework.response import Response
from rest_framework.views import APIView 
from rest_framework import status

from .models import Radar
from .serializers import RadarSerializer
# Create your views here.

class SpeedLogs(APIView):

    def get(self, request):
        data = Radar.objects.all().order_by('timestamp','velocidade')

        if request.headers.get('Accept') == 'application/json':
            serializer = RadarSerializer(data, many=True)
            return Response(serializer.data, status=status.HTTP_200_OK)

        return render(request, 'index.html', {'data': data})
    
    def post(self, request):
        print(request.data)
        serializer = RadarSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)
    
'''
{
"tempo":10,
"distancia": 10,
"velocidade": 1, 
"limite": False
}
'''