function I = DispDic(D,ImageType)
% %Display the dictionary elements as a image
% %Version 1: 02/02/2015
% %Written by Yunchen Pu, Duke ECE, yp42@duke.edu
% %The size of D is Nx * Ny * N for gray images and Nx * Ny *3* N for color images
% %Nx and Ny are the size of each image and N is the number of images
if nargin<2
    ImageType = 'gray';
end


%% load data
if strcmp(ImageType,'gray')==1
    [Nx,Ny,N]=size(D);
elseif strcmp(ImageType,'rgb')==1
    [Nx,Ny,~,N]=size(D);
else
    disp('error');
    return;
end
RowNum = fix(sqrt(N));
ColNum = ceil(N/RowNum);
widX = 1 / 1.2;
widY = 1 / 1.2;

subplot('position', [widX*0.1, widY * 0.1, widX, widY]);
I = zeros(RowNum*Nx + RowNum + 1, ColNum*Ny+ ColNum+1, 3);
I(:,:,3)=1;
for row = 1:RowNum
    for col = 1:ColNum
        n = (row-1)*ColNum + col;
        if n<=N
            if strcmp(ImageType,'gray')==1
                D(:,:,n)=bsxfun(@minus,D(:,:,n),min(min(D(:,:,n))));
                MaxD=squeeze(max(max(D(:,:,n))));
                if MaxD>0
                    D(:,:,n)=D(:,:,n)/MaxD;
                end
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 1) = D(:,:,n);
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 2) = D(:,:,n);
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 3) = D(:,:,n);
            else
                D(:,:,:,n)=bsxfun(@minus,D(:,:,:,n),min(min(min(D(:,:,:,n)))));
                MaxD=squeeze(max(max(max(D(:,:,:,n)))));
                if MaxD>0
                    D(:,:,:,n)=D(:,:,:,n)/MaxD;
                end
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 1) = D(:,:,1,n);
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 2) = D(:,:,2,n);
                I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 3) = D(:,:,3,n);
            end
        else
            I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 1) = 0;
            I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 2) = 0;
            I( (row-1)*(Nx+1)+2 : row*(Nx+1) , (col-1)*(Ny+1)+2 : col*(Ny+1), 3) = 0;
        end
    end
end
%imshow(I,'Border','tight'); axis equal; axis off;
imagesc(I)
