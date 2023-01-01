filename = 'output3.txt';
filename2 = 'hp.block';
contents = textread(filename,'%s','delimiter','\n','whitespace','');
contents2 = textread(filename2,'%s','delimiter','\n','whitespace','');
lines = [];
io = str2double(split(string(contents2(1))));
for i = 6:length(contents)
    lines = [lines string(contents(i))];
end
tmp = [];
for i = 1:length(lines)
    tmp = [tmp split(lines(i))];
end
name = tmp(1,:);
x = str2double(tmp(2,:));
y = str2double(tmp(3,:));
w = str2double(tmp(4,:))-x;
h = str2double(tmp(5,:))-y;
figure(1); 
for i = 1:length(w)
   rectangle('Position',[x(i),y(i),w(i),h(i)],'EdgeColor','#4DBEEE','FaceColor','#7E2F8E'); hold on;
end
text((x+w./2),(y+h./2),name,'Color','red','FontSize',8);
xline(io(2)); yline(io(3));