#include "ofxAlembicReader.h"

using namespace ofxAlembic;
using namespace Alembic::AbcGeom;

#pragma mark - IXform

ofxAlembic::IXform::IXform(Alembic::AbcGeom::IXform object) : ofxAlembic::IGeom(object), m_xform(object)
{
	TimeSamplingPtr iTsmp = m_xform.getSchema().getTimeSampling();
	if (!m_xform.getSchema().isConstant())
	{
		size_t numSamps =  m_xform.getSchema().getNumSamples();
		if (numSamps > 0)
		{
			chrono_t minTime = iTsmp->getSampleTime(0);
			m_minTime = std::min(m_minTime, minTime);
			chrono_t maxTime = iTsmp->getSampleTime(numSamps - 1);
			m_maxTime = std::max(m_maxTime, maxTime);
		}
	}
	
	type = XFORM;
}

ofxAlembic::IXform::~IXform()
{
	if (m_xform)
		m_xform.reset();
}

void ofxAlembic::IXform::updateWithTimeInternal(double time, Imath::M44f& transform)
{
	ISampleSelector ss(time, ISampleSelector::kNearIndex);

	M44f mat;
	M44d m = m_xform.getSchema().getValue(ss).getMatrix();
	double *src = m.getValue();
	float *dst = mat.getValue();

	for (int i = 0; i < 16; i++)
		dst[i] = src[i];

	transform = mat * transform;
	
	for (int i = 0; i < 16; i++)
	{
		xform.local_matrix.getPtr()[i] = mat.getValue()[i];
		xform.global_matrix.getPtr()[i] = transform.getValue()[i];
	}
}

#pragma mark - IPoints

ofxAlembic::IPoints::IPoints(Alembic::AbcGeom::IPoints object) : ofxAlembic::IGeom(object), m_points(object)
{
	TimeSamplingPtr iTsmp = m_points.getSchema().getTimeSampling();
	if (!m_points.getSchema().isConstant())
	{
		size_t numSamps =  m_points.getSchema().getNumSamples();
		if (numSamps > 0)
		{
			chrono_t minTime = iTsmp->getSampleTime(0);
			m_minTime = std::min(m_minTime, minTime);
			chrono_t maxTime = iTsmp->getSampleTime(numSamps - 1);
			m_maxTime = std::max(m_maxTime, maxTime);
		}
	}

	type = POINTS;
}

void ofxAlembic::IPoints::updateWithTimeInternal(double time, Imath::M44f& transform)
{
	points.set(m_points.getSchema(), time, transform);
}

#pragma mark - ICurves

ofxAlembic::ICurves::ICurves(Alembic::AbcGeom::ICurves object) : ofxAlembic::IGeom(object), m_curves(object)
{

	TimeSamplingPtr iTsmp = m_curves.getSchema().getTimeSampling();
	if (!object.getSchema().isConstant())
	{
		size_t numSamps = object.getSchema().getNumSamples();
		if (numSamps > 0)
		{
			chrono_t minTime = iTsmp->getSampleTime(0);
			m_minTime = std::min(m_minTime, minTime);
			chrono_t maxTime = iTsmp->getSampleTime(numSamps - 1);
			m_maxTime = std::max(m_maxTime, maxTime);
		}
	}

	type = CURVES;
}

void ofxAlembic::ICurves::updateWithTimeInternal(double time, Imath::M44f& transform)
{
	curves.set(m_curves.getSchema(), time, transform);
}

#pragma mark - IPolyMesh

ofxAlembic::IPolyMesh::IPolyMesh(Alembic::AbcGeom::IPolyMesh object) : ofxAlembic::IGeom(object), m_polyMesh(object)
{
	TimeSamplingPtr iTsmp = m_polyMesh.getSchema().getTimeSampling();
	if (!m_polyMesh.getSchema().isConstant())
	{
		size_t numSamps =  m_polyMesh.getSchema().getNumSamples();
		if (numSamps > 0)
		{
			chrono_t minTime = iTsmp->getSampleTime(0);
			m_minTime = std::min(m_minTime, minTime);
			chrono_t maxTime = iTsmp->getSampleTime(numSamps - 1);
			m_maxTime = std::max(m_maxTime, maxTime);
		}
	}

	type = POLYMESH;
}

void ofxAlembic::IPolyMesh::updateWithTimeInternal(double time, Imath::M44f& transform)
{
	polymesh.set(m_polyMesh.getSchema(), time, transform);
}

#pragma mark - ICamera

ofxAlembic::ICamera::ICamera(Alembic::AbcGeom::ICamera object) : ofxAlembic::IGeom(object), m_camera(object)
{
	TimeSamplingPtr iTsmp = m_camera.getSchema().getTimeSampling();
	if (!m_camera.getSchema().isConstant())
	{
		size_t numSamps =  m_camera.getSchema().getNumSamples();
		if (numSamps > 0)
		{
			chrono_t minTime = iTsmp->getSampleTime(0);
			m_minTime = std::min(m_minTime, minTime);
			chrono_t maxTime = iTsmp->getSampleTime(numSamps - 1);
			m_maxTime = std::max(m_maxTime, maxTime);
		}
	}
	
	type = CAMERA;
}

void ofxAlembic::ICamera::updateWithTimeInternal(double time, Imath::M44f& transform)
{
	camera.set(m_camera.getSchema(), time, transform);
}

#pragma mark - Reader

void ofxAlembic::IGeom::visit_geoms(ofPtr<IGeom> &obj, map<string, IGeom*> &object_map)
{
	for (int i = 0; i < obj->m_children.size(); i++)
		visit_geoms(obj->m_children[i], object_map);

	if (obj->isTypeOf(UNKHOWN)) return;

	object_map[obj->getName()] = obj.get();
}

bool ofxAlembic::Reader::open(string path)
{
	ofxAlembic::init();
	
	path = ofToDataPath(path);

	m_archive = IArchive(Alembic::AbcCoreHDF5::ReadArchive(), path);
	if (!m_archive.valid()) return false;

	m_root = ofPtr<IGeom>(new IGeom(m_archive.getTop()));

	{
		object_arr.clear();
		object_name_arr.clear();
		object_map.clear();

		ofxAlembic::IGeom::visit_geoms(m_root, object_map);

		map<string, IGeom*>::iterator it = object_map.begin();
		while (it != object_map.end())
		{
			object_arr.push_back(it->second);
			object_name_arr.push_back(it->first);
			it++;
		}
	}

	m_minTime = m_root->m_minTime;
	m_maxTime = m_root->m_maxTime;

	return true;
}

void ofxAlembic::Reader::close()
{
	object_arr.clear();
	object_name_arr.clear();
	object_map.clear();

	if (m_root)
		m_root.reset();

	if (m_archive.valid())
		m_archive.reset();
}

void ofxAlembic::Reader::draw()
{
	if (!m_root) return;

	m_root->draw();
}

void ofxAlembic::Reader::setTime(double time)
{
	if (!m_root) return;

	Imath::M44f m;
	m.makeIdentity();
	m_root->updateWithTime(time, m);

	current_time = time;
}

void ofxAlembic::Reader::dumpNames()
{
	const vector<string> &names = getNames();

	for (int i = 0; i < names.size(); i++)
	{
		cout << i << ": " << object_map[names[i]]->getTypeName() << " '" << names[i] << "'" << endl;
	}
}

bool ofxAlembic::Reader::get(const string& path, ofMatrix4x4& matrix)
{
	IGeom *o = get(path);
	if (o == NULL) return false;
	return o->get(matrix);
}

bool ofxAlembic::Reader::get(const string& path, ofMesh& mesh)
{
	IGeom *o = get(path);
	if (o == NULL) return false;
	return o->get(mesh);
}

bool ofxAlembic::Reader::get(const string& path, vector<ofPolyline>& curves)
{
	IGeom *o = get(path);
	if (o == NULL) return false;
	return o->get(curves);
}

bool ofxAlembic::Reader::get(const string& path, vector<ofVec3f>& points)
{
	IGeom *o = get(path);
	if (o == NULL) return false;
	return o->get(points);
}

bool ofxAlembic::Reader::get(const string& path, ofCamera &camera)
{
	IGeom *o = get(path);
	if (o == NULL) return false;
	return o->get(camera);
}

bool ofxAlembic::Reader::get(size_t idx, ofMatrix4x4& matrix)
{
	IGeom *o = get(idx);
	if (o == NULL) return false;
	return o->get(matrix);
}

bool ofxAlembic::Reader::get(size_t idx, ofMesh& mesh)
{
	IGeom *o = get(idx);
	if (o == NULL) return false;
	return o->get(mesh);
}

bool ofxAlembic::Reader::get(size_t idx, vector<ofPolyline>& curves)
{
	IGeom *o = get(idx);
	if (o == NULL) return false;
	return o->get(curves);
}

bool ofxAlembic::Reader::get(size_t idx, vector<ofVec3f>& points)
{
	IGeom *o = get(idx);
	if (o == NULL) return false;
	return o->get(points);
}

bool ofxAlembic::Reader::get(size_t idx, ofCamera &camera)
{
	IGeom *o = get(idx);
	if (o == NULL) return false;
	return o->get(camera);
}

#pragma mark - IGeom

IGeom::IGeom() : m_minTime(0), m_maxTime(0), type(UNKHOWN) {}

IGeom::IGeom(Alembic::AbcGeom::IObject object) : m_object(object), m_minTime(0), m_maxTime(0), type(UNKHOWN)
{
	type = UNKHOWN;
	setupWithObject(m_object);
}

IGeom::~IGeom()
{
	m_children.clear();

	if (m_object)
		m_object.reset();
}

void IGeom::setupWithObject(IObject object)
{
	size_t numChildren = object.getNumChildren();
	
	for (size_t i = 0; i < numChildren; ++i)
	{
		const ObjectHeader &ohead = object.getChildHeader(i);

		ofPtr<IGeom> dptr;
		if (Alembic::AbcGeom::IPolyMesh::matches(ohead))
		{
			Alembic::AbcGeom::IPolyMesh pmesh(object, ohead.getName());
			if (pmesh)
			{
				dptr.reset(new ofxAlembic::IPolyMesh(pmesh));
			}
		}
		else if (Alembic::AbcGeom::IPoints::matches(ohead))
		{
			Alembic::AbcGeom::IPoints points(object, ohead.getName());
			if (points)
			{
				dptr.reset(new ofxAlembic::IPoints(points));
			}
		}
		else if (Alembic::AbcGeom::ICurves::matches(ohead))
		{
			Alembic::AbcGeom::ICurves curves(object, ohead.getName());
			if (curves)
			{
				dptr.reset(new ofxAlembic::ICurves(curves));
			}
		}
		else if (Alembic::AbcGeom::INuPatch::matches(ohead))
		{
			ofLogError("ofxAlembic") << "INuPatch not implemented";
			assert(false);

//			Alembic::AbcGeom::INuPatch nuPatch(object, ohead.getName());
//			if ( nuPatch )
//			{
//				dptr.reset( new INuPatchDrw( nuPatch ) );
//			}
		}
		else if (Alembic::AbcGeom::IXform::matches(ohead))
		{
			Alembic::AbcGeom::IXform xform(object, ohead.getName());
			if (xform)
			{
				dptr.reset(new ofxAlembic::IXform(xform));
			}
		}
		else if (Alembic::AbcGeom::ISubD::matches(ohead))
		{
			ofLogError("ofxAlembic") << "ISubD not implemented";
			assert(false);

//			Alembic::AbcGeom::ISubD subd(object, ohead.getName());
//			if ( subd )
//			{
//				dptr.reset( new ISubDDrw( subd ) );
//			}
		}
		else if (Alembic::AbcGeom::ICamera::matches(ohead))
		{
			Alembic::AbcGeom::ICamera camera(object, ohead.getName());
			if (camera)
			{
				dptr.reset(new ofxAlembic::ICamera(camera));
			}
		}
		else
		{
			ofLogError("ofxAlembic") << "unknown object type: " << ohead.getFullName();
		}

		if (dptr && dptr->valid())
		{
			dptr->index = m_children.size();
			m_children.push_back(dptr);
			m_minTime = std::min(m_minTime, dptr->m_minTime);
			m_maxTime = std::max(m_maxTime, dptr->m_maxTime);
		}
	}
}

void IGeom::draw()
{
	drawInternal();

	for (int i = 0; i < m_children.size(); i++)
	{
		ofPtr<IGeom> c = m_children[i];
		c->draw();
	}
}

string IGeom::getName() const
{
	return m_object.getFullName();
}

void IGeom::updateWithTime(double time, Imath::M44f& transform)
{
	updateWithTimeInternal(time, transform);

	for (int i = 0; i < m_children.size(); i++)
	{
		Imath::M44f m = transform;
		m_children[i]->updateWithTime(time, m);
	}
}