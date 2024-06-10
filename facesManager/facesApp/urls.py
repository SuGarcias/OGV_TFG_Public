# facesApp/urls.py

from django.urls import path
from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('upload/', views.upload_image, name='upload_image'),
    path('edit/<str:doc_id>/', views.edit_user, name='edit_user'),
    path('delete/<str:doc_id>/', views.delete_user, name='delete_user'),
]
